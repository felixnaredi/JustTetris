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
  
  var corners: (bottomLeft: Position, bottomRight: Position, topLeft: Position, topRight: Position) {
    return (Position(x, y), Position(x + 1, y), Position(x, y + 1), Position(x + 1, y + 1))
  }
  
}


enum Formation {
  
  case Oh
  case Ie
  case Sa
  case Ze
  case Lo
  case Je
  case To
  
  init?(block: Block) {
    guard let formation = block.formation else { return nil }
    self = formation
  }
  
  init?(block: Block?) {
    guard let block = block else { return nil }
    self.init(block: block)
  }
  
  init?<BlockSequence: Sequence>(blocks: BlockSequence) where BlockSequence.Element == Block {
    self.init(block: blocks.first(where: { (block) -> Bool in return !block.isEmpty }))
  }
  
  init?(shape: Shape) {
    self.init(blocks: shape.blocks)
  }
  
}


struct Block {
  
  private struct Status {
    
    private let block: jsBlock
    
    init(_ block: jsBlock) { self.block = block }
    
    var isEmpty: Bool { return js_block_is_empty(block) }
    
    private var statusBits: UInt32 { return UInt32(block.status) }
    
    var formation: Formation? {
      switch js_block_formation(block) {
      case jsShapeFormationO: return .Oh
      case jsShapeFormationI: return .Ie
      case jsShapeFormationS: return .Sa
      case jsShapeFormationZ: return .Ze
      case jsShapeFormationL: return .Lo
      case jsShapeFormationJ: return .Je
      case jsShapeFormationT: return .To
      default: return nil
      }
    }
    
  }
  
  private let status: Status
  let position: Position
  
  init(_ block: jsBlock) {
    status = Status(block)
    position = Position(block.position.x, block.position.y)
  }
  
  var isEmpty: Bool { return status.isEmpty }
  
  var formation: Formation? { return status.formation }
  
  /// Static constant initializers
  /// ----------------------------------------------------------------
  static var empty: Block { return Block(js_empty_block()) }
  
}


struct BlockCollection: Collection {
  
  let buffer: UnsafeBufferPointer<jsBlock>
  
  var startIndex: Int { return buffer.startIndex }
  var endIndex: Int { return buffer.endIndex }
  
  subscript(index: Int) -> Block { return Block(buffer[index]) }
  
  func index(after i: Int) -> Int { return buffer.index(after: i) }
  
  /// A subscript interface for positional indicies.
  /// ----------------------------------------------------------------
  subscript(position: Position) -> Block? {
    return self.filter({ $0.position == position }).first
  }
  
  subscript(x: Int, y: Int) -> Block? { return self[Position(x, y)] }
  
}


struct Shape {
  
  static var blockAmount: Int { return Int(JS_SHAPE_BLOCK_AMOUNT) }
  static var rowAmount: Int { return Int(JS_SHAPE_ROW_AMOUNT) }
  static var columnAmount: Int { return Int(JS_SHAPE_COLUMN_AMOUNT) }
  
  let offset: Position
  let blocks: BlockCollection
  
  init(_ shape: jsShape) {
    offset = Position(shape.offset.x, shape.offset.y)
    blocks = BlockCollection(buffer: UnsafeBufferPointer(start: shape.blocks, count: Shape.blockAmount))
  }
  
}


extension Shape: CustomStringConvertible {
  
  private struct Descriptor {
    let offset: Position
    let blocks: [Block]
  }
  
  var description: String {
    return "Shape::\(Descriptor(offset: offset, blocks: Array(blocks)))"
  }
}


struct Board {
  
  static var blockAmount: Int { return Int(JS_BOARD_BLOCK_AMOUNT) }
  static var rowAmount: Int { return Int(JS_BOARD_ROW_AMOUNT) }
  static var columnAmount: Int { return Int(JS_BOARD_COLUMN_AMOUNT) }
  
  private let pointer: UnsafeMutablePointer<jsBoard>
  
  init(board: UnsafeMutablePointer<jsBoard>) { self.pointer = board }
  
  static var randomBoard: Board {
    let pointer = UnsafeMutablePointer<jsBoard>.allocate(capacity: MemoryLayout<jsBoard>.size)
    pointer.initialize(to: js_empty_board())
    
    let buffer = UnsafeMutableBufferPointer(start: pointer
      .withMemoryRebound(to: jsBlock.self, capacity: 1, { return $0 }), count: Board.blockAmount)
    
    buffer.enumerated().forEach { (offset, _) in
      buffer[offset] = js_rand_shape().blocks.pointee
      buffer[offset].position = jsVec2i(x: Int32(offset % Board.columnAmount), y: Int32(offset / Board.columnAmount))
    }
    
    return Board(board: pointer)
  }
  
  var blocks: BlockCollection {
    return BlockCollection(buffer: pointer
      .withMemoryRebound(to: jsBlock.self, capacity: 1) { (pointer) -> UnsafeBufferPointer<jsBlock> in
        
        return UnsafeBufferPointer(start: pointer, count: Board.blockAmount)
    })
  }
  
  var rows: [BlockCollection] {
    return (0..<Board.rowAmount).map { (i) -> BlockCollection in
      
      return BlockCollection(buffer: pointer
        .withMemoryRebound(to: jsRow.self, capacity: Board.rowAmount) { (pointer) -> UnsafeBufferPointer<jsBlock> in
          
          return UnsafeBufferPointer(start: pointer.advanced(by: i).withMemoryRebound(to: jsBlock.self, capacity: 1, { return $0 }),
                                     count: Board.columnAmount)
      })
    }
  }
  
}

