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
    private let fillEncoder: FillEncoder
    private let borderEncoder: BorderEncoder
    
    init?(context: RenderContext?, width: Int, height: Int) {
      guard let context = context else { return nil }
      
      guard let fill = FillEncoder(renderContext: context, gridDescriptor: TriangleFillGridDescriptor(width: width, height: height)) else { return nil }
      guard let border = BorderEncoder(renderContext: context, gridDescriptor: LineBorderGridDescriptor(width: width, height: height)) else { return nil }
      
      renderContext = context
      fillEncoder = fill
      borderEncoder = border
    }
    
    func encode(view: MTKView, _ body: (MTLRenderCommandEncoder, FillEncoder, BorderEncoder) -> Void) {
      // A view with a frame area of 0 will have a currentRenderPassDescriptor
      // that causes SIGABRT. Checking for it will prevent it without disrupting
      // the appearance of the drawing.
      guard (view.frame.width > 0 && view.frame.height > 0) else { return }
      
      guard let renderPassDescriptor = view.currentRenderPassDescriptor else { return }
      guard let commandBuffer = renderContext.commandQueue.makeCommandBuffer() else { return }
      
      guard let renderEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor) else { return }
      
      renderEncoder.setRenderPipelineState(renderContext.pipelineState)
      
      body(renderEncoder, fillEncoder, borderEncoder)
      
      renderEncoder.endEncoding()
      
      guard let drawable = view.currentDrawable else { return }
      
      commandBuffer.present(drawable)
      commandBuffer.commit()
    }
    
  }
  
  @IBOutlet var boardView: GridView!
  @IBOutlet var nextShapeView: GridView!

  private var boardDrawer: Drawer?
  private var nextShapeDrawer: Drawer?
  
  let gameCordinator = GameCordinator()
  
  @IBAction func breakPoint(_ sender: Any?) { }
  
  override func viewDidLoad() {
    super.viewDidLoad()
    
    if let context = RenderContext(with: boardView) {
      boardDrawer = Drawer(context: context, width: 10, height: 20)
    }
    
    if let context = RenderContext(with: nextShapeView) {
      nextShapeDrawer = Drawer(context: context, width: 4, height: 4)
    }
    
    nextShapeView.delegate = self
    nextShapeView.mouseDownHandler = { _ in self.gameCordinator.popShape() }
    
    boardView.delegate = self
    
    boardView.keyDownHandler = { (event) in
      print(event)
      
      switch event.keyCode {
      case 125:
        self.gameCordinator.translateShapeDown({ (result) in
          print(result.status)
          self.boardView.needsDisplay = true
        })
      case 123:
        self.gameCordinator.translateShapeLeft({ (result) in
          print(result.status)
          self.boardView.needsDisplay = true
        })
      case 124:
        self.gameCordinator.translateShapeRight({ (result) in
          print(result.status)
          self.boardView.needsDisplay = true
        })
      case 126:
        self.gameCordinator.rotateShapeClockwise({ (result) in
          print(result.status)
          self.boardView.needsDisplay = true
        })
      default:
        break
      }
    }
  }
  
  func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) { }
  
  func draw(in view: MTKView) {
    if view == boardView {
      boardDrawer?.encode(view: boardView) { (encoder, fill, border) in
        fill.encodeRenderCommands(for: gameCordinator.board.blocks, with: encoder)
        border.encodeRenderCommands(for: gameCordinator.board.blocks, with: encoder)
        
        fill.encodeRenderCommands(for: BlockArray(gameCordinator.shape.offsetedBlocks), with: encoder)
        border.encodeRenderCommands(for: BlockArray(gameCordinator.shape.offsetedBlocks), with: encoder)
      }
      
    } else if view == nextShapeView {
      nextShapeDrawer?.encode(view: nextShapeView) { (encoder, fill, border) in
        fill.encodeRenderCommands(for: gameCordinator.nextShape.blocks, with: encoder)
        border.encodeRenderCommands(for: gameCordinator.nextShape.blocks, with: encoder)
      }
    }
  }
  
}
