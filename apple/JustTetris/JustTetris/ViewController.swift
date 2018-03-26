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


class RedView: NSView {
  
  override func draw(_ dirtyRect: NSRect) {
    NSColor.red.setFill()
    NSBezierPath(rect: dirtyRect).fill()
  }
  
}


class ShapeView: MTKView {
  
  var mouseDownHandler: ((NSEvent) -> Void)?
  
  override func mouseDown(with event: NSEvent) {
    super.mouseDown(with: event)
    
    mouseDownHandler?(event)
  }
  
  override var clearColor: MTLClearColor {
    get { return MTLClearColor(red: 0, green: 0, blue: 0, alpha: 0) }
    set { }
  }
  
}


class ViewController: NSViewController {
  
  class ShapeViewDelegate: NSObject, MTKViewDelegate {
    
    typealias FillEncoder = GridRenderEncoder<TriangleFillGridDescriptor>
    typealias BorderEncoder = GridRenderEncoder<LineBorderGridDescriptor>
    
    var renderContext: RenderContext?
    
    var fillEncoder: FillEncoder?
    var borderEncoder: BorderEncoder?
    var blocks: Shape.BlockCollection?
    
    init(fill: FillEncoder?, border: BorderEncoder?) {
      fillEncoder = fill
      borderEncoder = border
    }
    
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) { }
    
    func draw(in view: MTKView) {
      // A view with a frame area of 0 will have a currentRenderPassDescriptor
      // that causes SIGABRT. Checking for it will prevent it without disrupting
      // the appearance of the drawing.
      guard (view.frame.width > 0 && view.frame.height > 0) else { return }
      
      guard let renderContext = renderContext else { return }
      guard let renderPassDescriptor = view.currentRenderPassDescriptor else { return }
      guard let commandBuffer = renderContext.commandQueue.makeCommandBuffer() else { return }
      
      guard let renderEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor) else { return }
      
      renderEncoder.setRenderPipelineState(renderContext.pipelineState)
      
      guard let blocks = blocks else { return }
      fillEncoder?.encodeRenderCommands(for: blocks, with: renderEncoder)
      borderEncoder?.encodeRenderCommands(for: blocks, with: renderEncoder)      
      
      renderEncoder.endEncoding()
      
      guard let drawable = view.currentDrawable else { return }
      
      commandBuffer.present(drawable)
      commandBuffer.commit()
    }
    
  }
  
  @IBOutlet var shapeView: ShapeView!
  
  var shapeViewDelegate: ShapeViewDelegate!
  
  var shape: Shape? {
    didSet {
      guard let shape = shape else { return }
      
      (shapeView.delegate as? ShapeViewDelegate)?.blocks = shape.blocks
      
      shapeView.needsDisplay = true
    }
  }
  
  @IBAction func breakPoint(_ sender: Any?) { }
  
  static func makeShape() -> Shape {
    return Shape(js_rand_shape())
  }
  
  func changeShape() { shape = ViewController.makeShape() }
  
  override func viewDidLoad() {
    guard let renderContext = RenderContext(with: shapeView) else { return }

    let fillRenderer = GridRenderEncoder(renderContext: renderContext, gridDescriptor: TriangleFillGridDescriptor(width: 4, height: 4))
    let borderRenderer = GridRenderEncoder(renderContext: renderContext, gridDescriptor: LineBorderGridDescriptor(width: 4, height: 4))
    
    shapeViewDelegate = ShapeViewDelegate(fill: fillRenderer, border: borderRenderer)
    shapeViewDelegate.renderContext = renderContext
    
    shapeView.mouseDownHandler = { _ in self.changeShape() }
    shapeView.delegate = shapeViewDelegate
    
    changeShape()
  }
  
}
