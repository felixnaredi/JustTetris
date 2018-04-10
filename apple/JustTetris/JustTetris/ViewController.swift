//
//  ViewController.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/8/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import Cocoa
import simd
import MetalKit


class GridView: MTKView {

  var mouseDownHandler: ((NSEvent) -> Void)?
  var keyDownHandler: ((NSEvent) -> Void)?

  override var clearColor: MTLClearColor {
    get { return MTLClearColor(red: 0, green: 0, blue: 0, alpha: 0) }
    set { }
  }

  override func mouseDown(with event: NSEvent) { mouseDownHandler?(event) }

  override func keyDown(with event: NSEvent) { keyDownHandler?(event) }

  override var acceptsFirstResponder: Bool { return true }

}


class ViewController: NSViewController, MTKViewDelegate {

  private struct Drawer {

    private let renderContext: RenderContext

    func gridEncoder<Descriptor: GridDescriptor>(descriptor: Descriptor) -> GridRenderEncoder<Descriptor>? {
      return GridRenderEncoder<Descriptor>(renderContext: renderContext, gridDescriptor: descriptor)
    }

    init?(context: RenderContext?) {
      guard let context = context else { return nil }
      renderContext = context
    }

    func encode(view: MTKView, _ body: (MTLRenderCommandEncoder) -> Void) {
      // A view with a frame area of 0 will have a currentRenderPassDescriptor
      // that causes SIGABRT. Checking for it will prevent it without disrupting
      // the appearance of the drawing.
      guard (view.frame.width > 0 && view.frame.height > 0) else { return }

      guard let renderPassDescriptor = view.currentRenderPassDescriptor else { return }
      guard let commandBuffer = renderContext.commandQueue.makeCommandBuffer() else { return }

      guard let renderEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor) else { return }

      renderEncoder.setRenderPipelineState(renderContext.pipelineState)

      body(renderEncoder)

      renderEncoder.endEncoding()

      guard let drawable = view.currentDrawable else { return }

      commandBuffer.present(drawable)
      commandBuffer.commit()
    }

  }

  @IBOutlet var boardView: GridView!
  @IBOutlet var nextShapeView: GridView!
  @IBOutlet var rowsLabel: NSTextField!
  @IBOutlet var scoreLabel: NSTextField!
  @IBOutlet var levelLabel: NSTextField!

  private var boardDrawer: Drawer?
  private var nextShapeDrawer: Drawer?

  let gameCordinator = GameCordinator()
  var timer: Timer?
  var gameOver = false

  @IBAction func breakPoint(_ sender: Any?) { }
  
  private func updateScore() {
    rowsLabel.stringValue = "Rows: \(gameCordinator.rowsCleared)"
    scoreLabel.stringValue = "Score: \(Int(gameCordinator.score))"
    levelLabel.stringValue = "Level: \(Int(gameCordinator.level))"    
  }
  
  private func resetScore() {
    rowsLabel.stringValue = "Rows: 0"
    scoreLabel.stringValue = "Score: 0"
    levelLabel.stringValue = "Level: 0"
  }

  private func scheduleTimer() -> Timer {
    return Timer.scheduledTimer(withTimeInterval: 0.25, repeats: true, block: { _ in
      let result = self.gameCordinator.translateShapeDown(user: false)        
      if result.didMerge { self.gameCordinator.popShape() }
      if result.gameOver { self.timer?.invalidate() }
      
      self.boardView.needsDisplay = true
      self.nextShapeView.needsDisplay = true
    })
  }

  @IBAction func newGame(_ sender: Any?) {
    gameCordinator.reset()
    gameOver = false
    resetScore()
    boardView.needsDisplay = true
    nextShapeView.needsDisplay = true
    timer?.invalidate()
    timer = scheduleTimer()
  }

  override func viewDidLoad() {
    super.viewDidLoad()

    if let context = RenderContext(with: boardView) {
      boardDrawer = Drawer(context: context)
    }

    if let context = RenderContext(with: nextShapeView) {
      nextShapeDrawer = Drawer(context: context)
    }

    nextShapeView.delegate = self
    nextShapeView.enableSetNeedsDisplay = true
    nextShapeView.mouseDownHandler = { _ in
      self.gameCordinator.popShape()
      self.boardView.needsDisplay = true
      self.nextShapeView.needsDisplay = true
    }

    boardView.delegate = self
    boardView.enableSetNeedsDisplay = true
    boardView.keyDownHandler = { (event) in
      switch event.keyCode {

      case 125: // down arrow
        let result = self.gameCordinator.translateShapeDown(user: true)
        if result.successfull {
          self.updateScore()
          self.boardView.needsDisplay = true
        }
        if result.didMerge {
          self.gameCordinator.popShape()
          self.boardView.needsDisplay = true
          self.nextShapeView.needsDisplay = true
        }
        if result.gameOver {
          self.gameOver = true
          self.timer?.invalidate()
        }

      case 123: // left arrow
        if self.gameCordinator
          .translateShapeLeft(user: true)
          .successfull { self.boardView.needsDisplay = true }

      case 124: // right arrow
        if self.gameCordinator
          .translateShapeRight(user: true)
          .successfull { self.boardView.needsDisplay = true }

      case 126: // up arrow
        if self.gameCordinator
          .rotateClockwise(user: true)
          .successfull { self.boardView.needsDisplay = true }

      case 49: // ' '
        func fall() {
          if self.gameCordinator
            .translateShapeDown(user: true)
            .successfull { fall() }
        }
        fall()
        
        self.updateScore()
        self.gameCordinator.popShape()
        self.boardView.needsDisplay = true
        self.nextShapeView.needsDisplay = true

      case 35: // 'p'
        if(self.timer?.isValid ?? false) { self.timer?.invalidate() }
        else { self.timer = self.scheduleTimer() }

      default:
        break
      }
    }

    newGame(self)
  }

  func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) { }

  func draw(in view: MTKView) {
    if view == boardView {
      boardDrawer?.encode(view: boardView) { (encoder) in
        boardDrawer?
          .gridEncoder(descriptor: TriangleFillGridDescriptor(width: Board.columnAmount, height: Board.rowAmount))?
          .encodeRenderCommands(for: gameCordinator.board.blocks, with: encoder)

        boardDrawer?
          .gridEncoder(descriptor: LineBorderGridDescriptor(width: Board.columnAmount, height: Board.rowAmount))?
          .encodeRenderCommands(for: gameCordinator.board.blocks, with: encoder)

        // Don't draw the shape if game over.
        guard gameOver == false else { return }

        boardDrawer?
          .gridEncoder(descriptor: TriangleFillGridDescriptor(width: Board.columnAmount, height: Board.rowAmount))?
          .encodeRenderCommands(for: gameCordinator.shape.offsetedBlocks, with: encoder)

        boardDrawer?
          .gridEncoder(descriptor: LineBorderGridDescriptor(width: Board.columnAmount, height: Board.rowAmount))?
          .encodeRenderCommands(for: gameCordinator.shape.offsetedBlocks, with: encoder)
      }
    } else if view == nextShapeView {
      nextShapeDrawer?.encode(view: nextShapeView) { (encoder) in
        nextShapeDrawer?
          .gridEncoder(descriptor: TriangleFillGridDescriptor(width: Shape.columnAmount, height: Shape.rowAmount))?
          .encodeRenderCommands(for: gameCordinator.nextShape.blocks, with: encoder)

        nextShapeDrawer?
          .gridEncoder(descriptor: LineBorderGridDescriptor(width: Shape.columnAmount, height: Shape.rowAmount))?
          .encodeRenderCommands(for: gameCordinator.nextShape.blocks, with: encoder)
      }
    }
  }

}
