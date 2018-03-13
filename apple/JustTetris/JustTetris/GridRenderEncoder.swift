//
//  GridRenderEncoder.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/13/18.
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


class GridRenderEncoder<GridDescriptorType: GridDescriptor> {
  
  typealias GridDescriptor = GridDescriptorType
  
  struct BufferIndex {
    
    static var matrix: Int { return Int(JS_BUFFER_INDEX_MATRIX) }
    static var verticies: Int { return Int(JS_BUFFER_INDEX_VERTICIES) }
    
  }
  
  let gridDescriptor: GridDescriptor
  
  let vertexBuffer: MTLBuffer
  let indexBuffer: MTLBuffer
  let matrixBuffer: MTLBuffer
  
  private typealias Index = GridDescriptorType.IndiciesLayout.Index
  private typealias Matrix = GridDescriptorType.Matrix
  private typealias BufferTuple = (matrixBuffer: MTLBuffer, indexBuffer: MTLBuffer, vertexBuffer: MTLBuffer)
  
  private static func makeIndexBuffer(with device: MTLDevice, indicies: [Index]) -> MTLBuffer? {
    return device.makeBuffer(bytes: indicies, length: MemoryLayout<Index>.stride * indicies.count, options: .cpuCacheModeWriteCombined)
  }
  
  private static func makeMatrixBuffer(with device: MTLDevice, matrix: Matrix) -> MTLBuffer? {
    let buffer = device.makeBuffer(length: MemoryLayout<Matrix>.size, options: .storageModeShared)
    buffer?.contents().storeBytes(of: matrix, as: Matrix.self)
    
    return buffer
  }
  
  private static func makeBuffers(with device: MTLDevice, descriptor: GridDescriptor) -> BufferTuple? {
    guard let matrixBuffer = self.makeMatrixBuffer(with: device, matrix: descriptor.matrix) else { return nil }
    guard let indexBuffer = self.makeIndexBuffer(with: device, indicies: descriptor.indicies) else { return nil }
    guard let vertexBuffer = device.makeBuffer(length: MemoryLayout<Vertex>.stride * descriptor.area * 4, options: .cpuCacheModeWriteCombined) else { return nil }
    
    return (matrixBuffer, indexBuffer, vertexBuffer)
  }
  
  init?(renderContext: RenderContext, gridDescriptor: GridDescriptor) {
    self.gridDescriptor = gridDescriptor
    
    guard let buffers = GridRenderEncoder.makeBuffers(with: renderContext.device, descriptor: gridDescriptor) else { return nil }
    
    vertexBuffer = buffers.vertexBuffer
    indexBuffer = buffers.indexBuffer
    matrixBuffer = buffers.matrixBuffer
  }
  
  func loadVertexBuffer(with blocks: GridDescriptor.BlockCollection) {
    gridDescriptor.loadVertexBuffer(vertexBuffer, with: blocks)
  }
  
  
  func encodeRenderCommands(for blocks: GridDescriptor.BlockCollection, with encoder: MTLRenderCommandEncoder)  {
    
    loadVertexBuffer(with: blocks)
    
    encoder.setVertexBuffer(vertexBuffer, offset: 0, index: BufferIndex.verticies)
    encoder.setVertexBuffer(matrixBuffer, offset: 0, index: BufferIndex.matrix)
    
    encoder.drawIndexedPrimitives(type: gridDescriptor.primitiveType,
                                  indexCount: gridDescriptor.indicies.count,
                                  indexType: GridDescriptorType.IndiciesLayout.metalIndexType,
                                  indexBuffer: indexBuffer,
                                  indexBufferOffset: 0)
  }
  
}

