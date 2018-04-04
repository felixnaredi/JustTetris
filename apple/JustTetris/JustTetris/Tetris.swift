//
//  Tetris.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/8/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//
// 1
// 1
// 2
// 6 Fibonacci
// 1e
// 1
// 1
// 2
// 6
// 1e
// f0  Fibonacci
// c30
// fff0
// 1
// 1
// 2
// 6
// 1e
// f0
// c30
// fff0
// 21fde0    Fibonacci
// 74d8b20
// 289f55e20
// 16d9a04f200
// 14cc12e804200
// 1
// 1
// 2
// 6
// 1e
// f0
// c30
// fff0
// 21fde0
// 74d8b20
// 289f55e20
// 16d9a04f200
// 14cc12e804200
// 1ea087d7ae13200             Fibonacci
// 48fa83afecc992400
// 1195dd9b745ed26dcc00
// 6db3e772c4f386b6f19c00
// 4534fe82e73fa17c696bea000
// 46a4a80afeac23864e5bf74b2000
// 74acd2caa8beae6c424b445ec3ca000
// 137cc58cfd43291df0ea96415bef8314000


import Foundation
import simd


typealias Position = int2

extension Position {
  
  init(_ x: Int, _ y: Int) { self.init(Int32(x), Int32(y)) }
  init(_ vector: jsVec2i) { self.init(vector.x, vector.y) }
  
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
    
    let block: jsBlock
    
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
  
  private init(_ block: jsBlock, at position: Position) {
    status = Status(block)
    self.position = position
  }
  
  var raw: jsBlock { return status.block }
  
  var isEmpty: Bool { return status.isEmpty }
  
  var formation: Formation? { return status.formation }
  
  func offseted(_ offset: Position) -> Block {
    return Block(raw, at: self.position &+ offset)
  }
  
  /// Static constant initializers
  /// ----------------------------------------------------------------
  static var empty: Block { return Block(js_empty_block()) }
  
}


struct Shape {
  
  static var blockAmount: Int { return Int(JS_SHAPE_BLOCK_AMOUNT) }
  static var rowAmount: Int { return Int(JS_SHAPE_ROW_AMOUNT) }
  static var columnAmount: Int { return Int(JS_SHAPE_COLUMN_AMOUNT) }
  
  let offset: Position
  let blocks: [Block]
  
  init(_ shape: jsShape) {
    offset = Position(shape.offset.x, shape.offset.y)
    blocks = UnsafeBufferPointer(start: shape.blocks, count: Shape.blockAmount).map { Block($0) }
  }

  var offsetedBlocks: [Block] {
    return blocks.map { (block) -> Block in return block.offseted(offset) }
  }
  
}


struct Board {
  
  static var blockAmount: Int { return Int(JS_BOARD_BLOCK_AMOUNT) }
  static var rowAmount: Int { return Int(JS_BOARD_ROW_AMOUNT) }
  static var columnAmount: Int { return Int(JS_BOARD_COLUMN_AMOUNT) }
  
  private let board: jsBoard
  
  init(board: jsBoard) { self.board = board }
  
  private func mapBlocks<T>(_ body: (jsBlock) -> T) -> [T] {
    var mutable = board.blocks
    // It really grinds my gears that 'UnsafePointer(&mutable)' causes 'Ambiguous use of init'.
    // That's why I first initializes with 'UnsafeMutablePointer'.
    return UnsafePointer(UnsafeMutablePointer(&mutable))
      .withMemoryRebound(to: jsBlock.self, capacity: 200, { (pointer) -> [T] in
        return UnsafeBufferPointer(start: pointer, count: Board.blockAmount).map(body)
      })
  }
  
  var blocks: [Block] { return mapBlocks { Block($0) } }
  
}

