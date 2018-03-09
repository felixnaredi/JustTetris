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


struct Vertex {
  
  private let vertex: js_shader_vertex
  
  var position: float2 { return vertex.position }
  var color: float4 { return vertex.color }
  
  init(position: float2, color: float4) {
    vertex = js_shader_vertex(position: position, color: color)
  }
  
}


struct BufferIndex {
  
  static var verticies: Int { return Int(JS_BUFFER_VERTICIES) }
  
}


struct RenderContext {
  
  let device: MTLDevice
  let pipelineState: MTLRenderPipelineState
  let commandQueue: MTLCommandQueue
  
  init?(with view: MTKView) {
    guard let device = MTLCreateSystemDefaultDevice() else {
      print("Device does not support Metal")
      return nil
    }
    
    self.device = device
    view.device = device
    
    guard let commandQueue = device.makeCommandQueue() else { return nil }
    self.commandQueue = commandQueue
    
    guard let library = device.makeDefaultLibrary() else { return nil }
    
    let descriptor = MTLRenderPipelineDescriptor()
    
    descriptor.vertexFunction = library.makeFunction(name: "vertexShader")
    descriptor.fragmentFunction = library.makeFunction(name: "fragmentShader")
    descriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat
    
    do { pipelineState = try device.makeRenderPipelineState(descriptor: descriptor) }
    catch let error {
      print(error)
      return nil
    }
  }
  
}


class Renderer: NSObject, MTKViewDelegate {
  
  let renderContext: RenderContext
  
  init(renderContext: RenderContext) {
    self.renderContext = renderContext
  }
  
  func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {
    return
  }
  
  private var verticies: [Vertex] {
    return [
      Vertex(position: float2( 0.8, -0.8), color: float4(1, 0, 0, 1)),
      Vertex(position: float2(-0.8, -0.8), color: float4(0, 1, 0, 1)),
      Vertex(position: float2( 0.0,  0.8), color: float4(0, 0, 1, 1)),
    ]
  }
  
  func draw(in view: MTKView) {
    guard let commandBuffer = renderContext.commandQueue.makeCommandBuffer() else { return }
    guard let renderPassDescriptor = view.currentRenderPassDescriptor else { return }
    
    guard let renderEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor) else { return }
    
    renderEncoder.setRenderPipelineState(renderContext.pipelineState)
    renderEncoder.setVertexBytes(verticies, length: MemoryLayout<Vertex>.size * 3, index: BufferIndex.verticies)
    
    renderEncoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: 3)
    
    renderEncoder.endEncoding()
    
    guard let drawable = view.currentDrawable else { return }
    
    commandBuffer.present(drawable)
    commandBuffer.commit()
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
  
  @IBOutlet var shapeView: ShapeView!
  
  var shapeViewRenderer: Renderer? {
    didSet { shapeView.delegate = shapeViewRenderer }
  }
  
  var shape: Shape?
  
  @IBAction func breakPoint(_ sender: Any?) { }
  
  static func makeShape() -> Shape? {
    guard let state = js_alloc_tetris_state() else { return nil }
    js_init_tetris_state(state)
    
    let shape = Shape(state.pointee.shape.pointee)
    
    js_dealloc_tetris_state(state)
    
    return shape
  }
  
  func changeShape() {
    shape = ViewController.makeShape()
    print(shape)
    shapeView.needsDisplay = true
  }
  
  override func viewDidLoad() {
    guard let shape = ViewController.makeShape() else {
      print("failed to make shape")
      return
    }
    
    print(shape)
    
    guard let renderContext = RenderContext(with: shapeView) else { return }
    shapeViewRenderer = Renderer(renderContext: renderContext)
    
    shapeView.mouseDownHandler = { _ in self.changeShape() }
  }
  
}
