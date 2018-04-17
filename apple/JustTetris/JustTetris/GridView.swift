//
//  GridView.swift
//  JustTetris
//
//  Created by Felix Naredi on 4/17/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//


import MetalKit


class GridView: MTKView {
  
  var grid: int2 { return int2(0, 0) }
  var blocks: [[UInt8]] = []
  
  private static func makeBlocks
    (grid: int2) -> [[UInt8]]
  {
    return Array(
      repeating: Array(
        repeatElement(0, count: Int(grid.x))
      ),
      count: Int(grid.y)
    )
  }
  
  required init(coder: NSCoder) {
    super.init(coder: coder)
    blocks = GridView.makeBlocks(grid: grid)
  }
  
  private func clearBlocks() {
    blocks = GridView.makeBlocks(grid: grid)
  }
  
  func addBlocks(_ blocks: [Block]) {
    blocks.forEach { (block) in
      self.blocks[Int(block.position.y)][Int(block.position.x)] =
        block.formation?.colorIndex ?? 0
    }
  }
  
  func setBlocks(with blocks: [Block]) {
    clearBlocks()
    addBlocks(blocks)
  }
  
  func render
    (commandBuffer: MTLCommandBuffer,
     renderPipelineState: MTLRenderPipelineState,
     _ body: (MTLRenderCommandEncoder) -> Void
    )
  {
    guard frame.width > 0 && frame.height > 0
    else { return }
    
    let renderEncoder = commandBuffer
      .makeRenderCommandEncoder(
        descriptor: currentRenderPassDescriptor!
    )!
    
    renderEncoder
      .setRenderPipelineState(renderPipelineState)
    
    body(renderEncoder)
    renderEncoder.endEncoding()
    
    commandBuffer.present(currentDrawable!)
    commandBuffer.commit()
  }
  
}


class ShapeView: GridView {
  
  override var grid: int2 {
    return int2(
      Shape.columnAmount,
      Shape.rowAmount
    )
  }
  
  var mouseDownHandler: ((NSEvent) -> Void)?
  
  override func mouseDown(with event: NSEvent) {
    mouseDownHandler?(event)
  }
  
}


class BoardView: GridView {
  
  override var grid: int2 {
    return int2(
      Board.columnAmount,
      Board.rowAmount
    )
  }
  
  var keyDownHandler: ((NSEvent) -> Void)?
  
  override var acceptsFirstResponder: Bool {
    return true
  }
  
  override func keyDown(with event: NSEvent) {
    keyDownHandler?(event)
  }
  
}
