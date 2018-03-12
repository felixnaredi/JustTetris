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

protocol RectangleCornerLayout {
  
  typealias IndexType = uint16
  
  static var bottomLeftCornerOffset: IndexType { get }
  static var bottomRightCornerOffset: IndexType { get }
  static var topLeftCornerOffset: IndexType { get }
  static var topRightCornerOffset: IndexType { get }
  
}


extension RectangleCornerLayout {
  
  static func triangleIndicies() -> [IndexType] {
    return [bottomLeftCornerOffset, topRightCornerOffset, topLeftCornerOffset,
            bottomLeftCornerOffset, bottomRightCornerOffset, topRightCornerOffset]
  }
  
  static func triangleIndicies(fromRectangle begin: Int) -> [IndexType] {
    return triangleIndicies().map { index in return IndexType(begin) * 4 + index }
  }
  
  static func lineIndicies() -> [IndexType] {
    return [bottomLeftCornerOffset, bottomRightCornerOffset,
            bottomRightCornerOffset, topRightCornerOffset,
            topRightCornerOffset, topLeftCornerOffset,
            topLeftCornerOffset, bottomLeftCornerOffset]
  }
  
  static func lineIndicies(fromRectangle begin: Int) -> [IndexType] {
    return lineIndicies().map { index in return IndexType(begin) * 4 + index }
  }
  
  static var metalIndexType: MTLIndexType { return MTLIndexType.uint16 }
  
}


protocol GridDescriptor {
  
  associatedtype BlockCollection: Collection where BlockCollection.Element == Block
  associatedtype VertexCollection: Collection where VertexCollection.Element == Vertex
  associatedtype IndiciesLayout: RectangleCornerLayout
  
  var width: Int { get }
  var height: Int { get }
  var primitiveType: MTLPrimitiveType { get }
  
  var matrixBuffer: MTLBuffer { get }
  var matrixBufferIndex: Int { get }
  
  var indexBuffer: MTLBuffer { get }
  
  init?(with device: MTLDevice)
  
  func verticies(for blocks: BlockCollection) -> VertexCollection
  
}


fileprivate extension GridDescriptor {
  
  typealias Index = IndiciesLayout.IndexType
  typealias Matrix = float4x4
  
  private static func scalar(width: Int, height: Int) -> Matrix {
    let w = 2 / Float(width)
    let h = 2 / Float(height)
    
    return Matrix(float4(w, 0, 0, 0),
                  float4(0, h, 0, 0),
                  float4(0, 0, 1, 0),
                  float4(0, 0, 0, 1))
  }
  
  private static var translate: Matrix {
    return Matrix(float4(1, 0, 0, -1),
                  float4(0, 1, 0, -1),
                  float4(0, 0, 1,  0),
                  float4(0, 0, 0,  1))
  }
  
  static func matrix(width: Int, height: Int) -> Matrix {
    return scalar(width: width, height: height) * translate
  }
  
  private static func triangleIndicies(width: Int, height: Int) -> [Index] {
    return Array((0..<(width * height)).map { (n) -> [Index] in
      return IndiciesLayout.triangleIndicies(fromRectangle: n)
      
      }.joined())
  }
  
  private static func lineIndicies(width: Int, height: Int) -> [Index] {
    return Array((0..<(width * height)).map { (n) -> [Index] in
      return IndiciesLayout.lineIndicies(fromRectangle: n)
      
    }.joined())
  }
  
  static func indicies(width: Int, height: Int, primitiveType: MTLPrimitiveType) -> [Index] {
    switch primitiveType {
    case .triangle: return triangleIndicies(width: width, height: height)
    case .line: return lineIndicies(width: width, height: height)
    default: return []
    }
  }
  
  var indicies: [IndiciesLayout.IndexType] {
    return Self.indicies(width: width, height: height, primitiveType: primitiveType)
  }
  
  var matrix: Matrix { return Self.matrix(width: width, height: height) }
  
  static func makeMatrixBuffer(with device: MTLDevice, matrix: Matrix) -> MTLBuffer? {
    let buffer = device.makeBuffer(length: MemoryLayout<Matrix>.size, options: .storageModeShared)
    buffer?.contents().storeBytes(of: matrix, as: Matrix.self)
    
    return buffer
  }
  
  static func makeIndexBuffer(with device: MTLDevice, indicies: [Index]) -> MTLBuffer? {
    guard let buffer = device.makeBuffer(length: MemoryLayout<Index>.stride * indicies.count, options: .storageModeShared) else { return nil }

    let data = Data(buffer: indicies.withUnsafeBufferPointer { return $0 })
    data.copyBytes(to: UnsafeMutablePointer<UInt8>(OpaquePointer(buffer.contents())), count: data.count)
    
    return buffer
  }
  
  static func makeBuffers(with device: MTLDevice, width: Int, height: Int, primitiveType: MTLPrimitiveType) -> (matrixBuffer: MTLBuffer, indexBuffer: MTLBuffer)? {
    guard let matrixBuffer = Self.makeMatrixBuffer(with: device, matrix: Self.matrix(width: width, height: height)) else { return nil }
    guard let indexBuffer = Self.makeIndexBuffer(with: device, indicies: Self.indicies(width: width, height: height, primitiveType: primitiveType)) else { return nil }
    
    return (matrixBuffer, indexBuffer)
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
    
    renderEncoder.drawIndexedPrimitives(type: gridDescriptor.primitiveType,
                                        indexCount: gridDescriptor.indicies.count,
                                        indexType: GridDescriptorType.IndiciesLayout.metalIndexType,
                                        indexBuffer: gridDescriptor.indexBuffer,
                                        indexBufferOffset: 0)
    
    renderEncoder.endEncoding()
    
    guard let drawable = view.currentDrawable else { return }
    
    commandBuffer.present(drawable)
    commandBuffer.commit()
  }
  
}


fileprivate enum Color {
  
  case red
  case yellow
  case green
  case cyan
  
  var value: float4 {
    switch self {
    case .red: return float4(1, 0, 0, 1)
    case .yellow: return float4(1, 1, 0, 1)
    case .green: return float4(0, 1, 0, 1)
    case .cyan: return float4(0, 1, 1, 1)
    }
  }
  
  var shadow: float4 { return self.value - float4(0.2, 0.2, 0.2, 0) }
  
}


struct ShapeGridDescriptor: GridDescriptor {
  
  typealias VertexCollection = FlattenBidirectionalCollection<[[Vertex]]>
  
  struct IndiciesLayout: RectangleCornerLayout {
    
    typealias IndexType = uint16
    
    static var bottomLeftCornerOffset: IndexType { return 0 }
    static var bottomRightCornerOffset: IndexType { return 1 }
    static var topLeftCornerOffset: IndexType { return 2 }
    static var topRightCornerOffset: IndexType { return 3 }
    
  }
  
  let width: Int = 4
  let height: Int = 4
  let primitiveType = MTLPrimitiveType.line
  
  let matrixBuffer: MTLBuffer
  let matrixBufferIndex: Int = GraphicResource.shapeMatrix
  
  let indexBuffer: MTLBuffer
  
  init?(with device: MTLDevice) {
    guard let buffers = ShapeGridDescriptor.makeBuffers(with: device, width: width, height: height, primitiveType: primitiveType) else { return nil }
    
    matrixBuffer = buffers.matrixBuffer
    indexBuffer = buffers.indexBuffer
  }
  
  func verticies(for blocks: Shape.BlockCollection) -> VertexCollection {
    return zip(blocks.filter({ block in return !block.status.empty }), [Color.red, Color.yellow, Color.green, Color.cyan])
      
    .map({ (pair) -> [Vertex] in
      let (block, color) = pair
      
      let (bl, br, tl, tr) = block.position.corners()
      
      return [Vertex(position: float2(Float(bl.x), Float(bl.y)), color: color.shadow),
              Vertex(position: float2(Float(br.x), Float(br.y)), color: color.shadow),
              Vertex(position: float2(Float(tl.x), Float(tl.y)), color: color.value),
              Vertex(position: float2(Float(tr.x), Float(tr.y)), color: color.value)]
    }).joined()
  }
  
}

typealias ShapeRenderer = GridRenderer<ShapeGridDescriptor>


