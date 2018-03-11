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

extension Position {
  
  init(_ x: Int, _ y: Int) { self.init(Int32(x), Int32(y)) }
  
  func corners() -> (bottomLeft: Position, bottomRight: Position, topLeft: Position, topRight: Position) {
    return (Position(x, y), Position(x + 1, y), Position(x, y + 1), Position(x + 1, y + 1))
  }
  
}

struct Block {
  
  struct Status {
    let empty: Bool
    
    init(for block: jsBlock) {
      empty = js_block_is_empty(block)
    }
    
  }
  
  let status: Status
  let position: Position
  
  init(_ block: jsBlock) {
    status = Status(for: block)
    position = Position(block.position.x, block.position.y)
  }
  
  /// Static constant initializers
  /// ----------------------------------------------------------------
  static var empty: Block { return Block(js_empty_block()) }
  
}


struct Shape {
  
  struct BlockCollection: Collection {
    
    private let blockArray: UnsafePointer<jsBlock>
    
    var blocks: [Block] { return map { return $0 } }
    
    init(_ blocks: UnsafePointer<jsBlock>) { blockArray = blocks }
    
    /// Implementing protocol Collection
    /// ----------------------------------------------------------------
    var startIndex: Int { return 0 }
    var endIndex: Int { return Shape.blockAmount }
    
    subscript(index: Int) -> Block { return Block(blockArray.advanced(by: index).pointee) }
    
    func index(after i: Int) -> Int { return i + 1 }
    
    /// A subscript interface for positional indicies.
    /// ----------------------------------------------------------------
    static func index(for position: Position) -> Int {
      return Int(position.y) * Shape.columnAmount + Int(position.x)
    }
    
    static func position(for index: Int) -> Position {
      return Position(index % Shape.columnAmount, index / Shape.columnAmount)
    }
    
    subscript(position: Position) -> Block? {
      return self.filter({ $0.position == position }).first
    }
    
    subscript(x: Int, y: Int) -> Block? { return self[Position(x, y)] }
    
  }
  
  static let blockAmount = Int(JS_SHAPE_BLOCK_AMOUNT)
  static let rowAmount = Int(JS_SHAPE_ROW_AMOUNT)
  static let columnAmount = Int(JS_SHAPE_COLUMN_AMOUNT)
  
  let offset: Position
  let blocks: BlockCollection
  
  init(_ shape: jsShape) {
    offset = Position(shape.offset.x, shape.offset.y)
    blocks = BlockCollection(shape.blocks)    
  }
  
}


extension Shape: CustomStringConvertible {
  
  private struct Descriptor {
    let offset: Position
    let blocks: [Block]
  }
  
  var description: String {
    return "Shape::\(Descriptor(offset: offset, blocks: blocks.blocks))"
  }
}
