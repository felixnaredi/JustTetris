//
//  GridRenderer.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/10/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import MetalKit


struct Vertex {
  
  private let vertex: js_shader_vertex
  
  var position: float2 { return vertex.position }
  var color: float4 { return vertex.color }
  
  init(position: float2, color: float4) {
    vertex = js_shader_vertex(position: position, color: color)
  }
  
}


struct GraphicResource {
  
  static var verticies: Int { return Int(JS_GRAPHIC_RESOURCE_VERTICIES) }
  
  static var shapeMatrix: Int { return Int(JS_GRAPHIC_RESOURCE_SHAPE_MATRIX) }
  
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


protocol GridDescriptor {
  
  associatedtype BlockCollection: Collection where BlockCollection.Element == Block
  associatedtype VertexCollection: Collection where VertexCollection.Element == Vertex
  
  var width: Int { get }
  var height: Int { get }
  var matrixBuffer: MTLBuffer { get }
  var matrixBufferIndex: Int { get }
  
  init?(with device: MTLDevice)
  
  func verticies(for blocks: BlockCollection) -> VertexCollection
  
}


extension GridDescriptor {
  typealias Matrix = float4x4
  
  var scalar: Matrix {
    let w = 2 / Float(width)
    let h = 2 / Float(height)
    
    return Matrix(float4(w, 0, 0, 0),
                  float4(0, h, 0, 0),
                  float4(0, 0, 1, 0),
                  float4(0, 0, 0, 1))
  }
  
  var translate: Matrix {
    return Matrix(float4(1, 0, 0, -1),
                  float4(0, 1, 0, -1),
                  float4(0, 0, 1,  0),
                  float4(0, 0, 0,  1))
  }

  var matrix: Matrix {
    return scalar * translate
  }
    
}


class GridRenderer<GridDescriptorType: GridDescriptor>: NSObject, MTKViewDelegate {
  
  let renderContext: RenderContext
  let gridDescriptor: GridDescriptorType
  
  var blocks: GridDescriptorType.BlockCollection!
  
  init(renderContext: RenderContext, gridDescriptor: GridDescriptorType) {
    self.renderContext = renderContext
    self.gridDescriptor = gridDescriptor
    
    super.init()
  }
  
  func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) { }
  
  func draw(in view: MTKView) {
    // A view with a frame area of 0 will have a currentRenderPassDescriptor
    // that causes SIGABRT. Checking for it will prevent it without disrupting
    // the appearance of the drawing.
    guard (view.frame.width > 0 && view.frame.height > 0) else { return }
    
    guard let commandBuffer = renderContext.commandQueue.makeCommandBuffer() else { return }
    guard let renderPassDescriptor = view.currentRenderPassDescriptor else { return }
    
    guard let renderEncoder = commandBuffer.makeRenderCommandEncoder(descriptor: renderPassDescriptor) else { return }
    
    let verticies = Array(gridDescriptor.verticies(for: blocks))
    
    renderEncoder.setRenderPipelineState(renderContext.pipelineState)
    renderEncoder.setVertexBytes(verticies, length: MemoryLayout<Vertex>.stride * verticies.count, index: GraphicResource.verticies)
    renderEncoder.setVertexBuffer(gridDescriptor.matrixBuffer, offset: 0, index: gridDescriptor.matrixBufferIndex)
    
    renderEncoder.drawPrimitives(type: .triangleStrip, vertexStart: verticies.startIndex, vertexCount: verticies.count)
    
    renderEncoder.endEncoding()
    
    guard let drawable = view.currentDrawable else { return }
    
    commandBuffer.present(drawable)
    commandBuffer.commit()
  }
  
}


struct ShapeGridDescriptor: GridDescriptor {

  typealias BlockCollection = Shape.BlockCollection
  typealias VertexCollection = FlattenBidirectionalCollection<[[Vertex]]>
  
  let width: Int = 4
  let height: Int = 4
  let matrixBufferIndex: Int = GraphicResource.shapeMatrix
  
  let matrixBuffer: MTLBuffer
  
  init?(with device: MTLDevice) {
    
    guard let matrixBuffer = device.makeBuffer(length: MemoryLayout<GridDescriptor.Matrix>.stride, options: .storageModeShared) else { return nil }
    self.matrixBuffer = matrixBuffer
    
    matrixBuffer.contents().storeBytes(of: matrix, as: GridDescriptor.Matrix.self)
  }
  
  func verticies(for blocks: Shape.BlockCollection) -> VertexCollection {
    return blocks.filter({ block in return !block.status.empty }).map({ (block) -> [Vertex] in
      let (bl, br, tl, tr) = block.position.corners()
      
      return [Vertex(position: float2(Float(bl.x), Float(bl.y)), color: float4(0.7, 0.7, 0.1, 1.0)),
              Vertex(position: float2(Float(br.x), Float(br.y)), color: float4(0.7, 0.7, 0.1, 1.0)),
              Vertex(position: float2(Float(tl.x), Float(tl.y)), color: float4(0.8, 0.8, 0.1, 1.0)),
              Vertex(position: float2(Float(tr.x), Float(tr.y)), color: float4(0.8, 0.8, 0.1, 1.0))]
    }).joined()
  }
  
}


class ShapeRenderer: GridRenderer<ShapeGridDescriptor> {

}

