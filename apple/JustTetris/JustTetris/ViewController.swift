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
    
    typealias FillEncoder = GridRenderEncoder<TriangleFillGridDescriptor>
    typealias BorderEncoder = GridRenderEncoder<LineBorderGridDescriptor>
    
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

  private var boardDrawer: Drawer?
  private var nextShapeDrawer: Drawer?
  
  let gameCordinator = GameCordinator()
  var timer: Timer?
  var rowsCleared = 0
  var score: Float = 0.0
  
  @IBAction func breakPoint(_ sender: Any?) { }
  
  @IBAction func emptyBoard(_ sender: Any?) {
    gameCordinator.emptyBoard()
    boardView.needsDisplay = true
  }
  
  private func addScore(for result: GameCordinator.TranslationResult) {
    guard result.status == .success else { return }
    score += result.score
    scoreLabel.stringValue = "Score: \(Int(score))"
  }
  
  private func addScore(for result: GameCordinator.ClearRowsResult) {
    rowsCleared += result.amount
    rowsLabel.stringValue = "Rows: \(rowsCleared)"
    score += result.score
    scoreLabel.stringValue = "Score: \(Int(score))"
  }
  
  private func translateDownHandler(_ result: GameCordinator.TranslationResult) -> (Bool, Bool) {
    switch(result.status) {
    case .success:
      return (true, false)
      
    case .merge:
      self.gameCordinator.mergeShape()
      let _ = self.gameCordinator.clearRows(
        boardWillChange: self.addScore,
        boardDidChange: { _ in }
      )      
      self.gameCordinator.popShape()
      
      return (true, true)
      
    default:
      return (false, false)
    }
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
    nextShapeView.mouseDownHandler = { _ in self.gameCordinator.popShape() }
    
    boardView.delegate = self
    boardView.enableSetNeedsDisplay = true    
    boardView.keyDownHandler = { (event) in
      switch event.keyCode {
        
      case 125:
        let (boardViewNeedsDisplay, nextShapeViewNeedsDisplay) = self.gameCordinator.translateShapeDown(
          shapeWillChange: self.addScore,
          shapeDidChange: self.translateDownHandler
          
        ).shapeDidChange!
        
        self.boardView.needsDisplay = boardViewNeedsDisplay
        self.nextShapeView.needsDisplay = nextShapeViewNeedsDisplay
        
      case 123:
        self.boardView.needsDisplay = self.gameCordinator.translateShapeLeft(
          shapeWillChange: { _ in },
          shapeDidChange: { (result) -> Bool in return result.status == .success }
          
        ).shapeDidChange!
        
      case 124:
        self.boardView.needsDisplay = self.gameCordinator.translateShapeRight(
          shapeWillChange: { _ in },
          shapeDidChange: { (result) -> Bool in return result.status == .success }
          
        ).shapeDidChange!
        
      case 126:
        self.boardView.needsDisplay = self.gameCordinator.rotateShapeClockwise(
          shapeWillChange: { _ in },
          shapeDidChange: { (result) -> Bool in return result.status == .success }
          
        ).shapeDidChange!
        
      case 49:
        func fall() {
          let _ = self.gameCordinator.translateShapeDown(
            shapeWillChange: self.addScore,
            
            shapeDidChange: { (result) in
              guard result.status == .success else { return }
              let _ = self.translateDownHandler(result)
              fall()
          })
        }
        
        fall()
        
        let (boardViewNeedsDisplay, nextShapeViewNeedsDisplay) = self.gameCordinator.translateShapeDown(
          shapeWillChange: { _ in },
          shapeDidChange: self.translateDownHandler
          
          ).shapeDidChange!
        
        self.boardView.needsDisplay = boardViewNeedsDisplay
        self.nextShapeView.needsDisplay = nextShapeViewNeedsDisplay
        
      default:
        break
      }
    }
    
    timer = Timer.scheduledTimer(withTimeInterval: 0.25, repeats: true, block: { _ in
      let (boardViewNeedsDisplay, nextShapeViewNeedsDisplay) = self.gameCordinator.translateShapeDown(
        shapeWillChange: { _ in },
        shapeDidChange: self.translateDownHandler
        
        ).shapeDidChange!
      
      self.boardView.needsDisplay = boardViewNeedsDisplay
      self.nextShapeView.needsDisplay = nextShapeViewNeedsDisplay
    })
  }
  
  func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) { }
  
  func draw(in view: MTKView) {
    if view == boardView {
      boardDrawer?.encode(view: boardView) { (encoder) in
        boardDrawer?
          .gridEncoder(descriptor: TriangleFillGridDescriptor(width: Board.columnAmount, height: Board.rowAmount))?
          .encodeRenderCommands(for: BlockArray(gameCordinator.shape.offsetedBlocks), with: encoder)
        
        boardDrawer?
          .gridEncoder(descriptor: LineBorderGridDescriptor(width: Board.columnAmount, height: Board.rowAmount))?
          .encodeRenderCommands(for: BlockArray(gameCordinator.shape.offsetedBlocks), with: encoder)
        
        boardDrawer?
          .gridEncoder(descriptor: TriangleFillGridDescriptor(width: Board.columnAmount, height: Board.rowAmount))?
          .encodeRenderCommands(for: gameCordinator.board.blocks, with: encoder)
        
        boardDrawer?
          .gridEncoder(descriptor: LineBorderGridDescriptor(width: Board.columnAmount, height: Board.rowAmount))?
          .encodeRenderCommands(for: gameCordinator.board.blocks, with: encoder)
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
