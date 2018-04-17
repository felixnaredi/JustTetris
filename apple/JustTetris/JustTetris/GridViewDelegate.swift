//
//  GridViewDelegate.swift
//  JustTetris
//
//  Created by Felix Naredi on 4/17/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import MetalKit


fileprivate func makeRenderPipelineState
  (with device: MTLDevice)
  throws -> MTLRenderPipelineState
{
  let descriptor = MTLRenderPipelineDescriptor()
  descriptor
    .colorAttachments[0]
      .pixelFormat = .bgra8Unorm
  
  let library = device.makeDefaultLibrary()
  descriptor.vertexFunction = library?
    .makeFunction(name: "vertexShader")
  descriptor.fragmentFunction = library?
    .makeFunction(name: "fragmentShader")
  
  return try device.makeRenderPipelineState(descriptor: descriptor)
}

fileprivate func setVertexBytes<Element>
  (with encoder: MTLRenderCommandEncoder,
   array: [Element],
   index: Int
  )
{
  array.withUnsafeBytes { (pointer) in
    encoder.setVertexBytes(
      pointer.baseAddress!,
      length: pointer.count,
      index: index
    )
  }
}


class GridViewDelegate: NSObject, MTKViewDelegate {
  
  let device = MTLCreateSystemDefaultDevice()!
  let commandQueue: MTLCommandQueue
  let renderPipelineState: MTLRenderPipelineState?
  
  override init() {
    commandQueue = device.makeCommandQueue()!
    renderPipelineState = try? makeRenderPipelineState(with: device)
    
    super.init()
  }
  
  func mtkView
    (_ view: MTKView,
     drawableSizeWillChange size: CGSize
    )
  { }
  
  func draw(in view: GridView) {
    view.render(
      commandBuffer: commandQueue.makeCommandBuffer()!,
      renderPipelineState: renderPipelineState!
    ) { (encoder) in
      
      setVertexBytes(
        with: encoder,
        array: [
          float2(-1, -1),
          float2( 1, -1),
          float2(-1,  1),
          float2( 1,  1),
        ],
        index: 0
      )
      
      let grid = view.grid
      
      setVertexBytes(with: encoder, array: [grid], index: 1)
      
      setVertexBytes(
        with: encoder,
        array: [UInt8](view.blocks.joined()),
        index: 2
      )
      
      setVertexBytes(
        with: encoder,
        array: [
          float3(0.1, 0.1, 0.1),
          float3(0.9, 0.0, 0.0),
          float3(0.9, 0.9, 0.0),
          float3(0.0, 0.9, 0.0),
          float3(0.0, 0.9, 0.9),
          float3(0.0, 0.0, 0.9),
          float3(0.9, 0.0, 0.9),
          float3(0.9, 0.9, 0.9),
        ],
        index: 3
      )
      
      encoder.drawPrimitives(
        type: .triangleStrip,
        vertexStart: 0,
        vertexCount: 4,
        instanceCount: Int(grid.x * grid.y)
      )
    }
  }
  
  func draw(in view: MTKView) {
    guard let gridView = (view as? GridView)
    else { return }
    draw(in: gridView)
  }
  
  
  
}
