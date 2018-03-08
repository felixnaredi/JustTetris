//
//  Tetris.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/8/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import Foundation
import simd


typealias Position = int2


struct Block {
  
  let status: Int32
  let position: Position
  
  init(_ block: jsBlock) {
    status = block.status
    position = Position(block.position.x, block.position.y)
  }
  
}


struct ShapeBlockCollection: Collection {
  
  private let blockArray: UnsafePointer<jsBlock>
  
  var blocks: [Block] { return map { return $0 } }
  
  init(_ blocks: UnsafePointer<jsBlock>) { blockArray = blocks }
  
  /// Implementing protocol Collection
  /// ----------------------------------------------------------------
  var startIndex: Int { return 0 }
  var endIndex: Int { return Int(JS_SHAPE_BLOCK_AMOUNT) }
  
  subscript(index: Int) -> Block { return Block(blockArray.advanced(by: index).pointee) }
  
  func index(after i: Int) -> Int { return i + 1 }
  
}


struct Shape {
  
  static let rowAmount = Int(JS_SHAPE_ROW_AMOUNT)
  static let columnAmount = Int(JS_SHAPE_COLUMN_AMOUNT)
  
  let offset: Position
  let blocks: [Block]
  
  init(_ shape: jsShape) {
    offset = Position(shape.offset.x, shape.offset.y)
    blocks = ShapeBlockCollection(shape.blocks).blocks
  }
  
}
