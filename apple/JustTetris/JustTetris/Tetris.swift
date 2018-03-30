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


protocol BlockCollection: Collection where Self.Element == Block { }


struct BlockBuffer: BlockCollection {
  
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


struct BlockArray: BlockCollection {
  
  private let array: Array<Block>
  
  init(_ other: Array<Block>) { array = other }
  
  var startIndex: Int { return array.startIndex }
  var endIndex: Int { return array.endIndex }
  
  subscript(index: Int) -> Block { return array[index] }
  
  func index(after i: Int) -> Int { return array.index(after: i) }
  
}


struct Shape {
  
  static var blockAmount: Int { return Int(JS_SHAPE_BLOCK_AMOUNT) }
  static var rowAmount: Int { return Int(JS_SHAPE_ROW_AMOUNT) }
  static var columnAmount: Int { return Int(JS_SHAPE_COLUMN_AMOUNT) }
  
  let offset: Position
  let blocks: BlockBuffer
  
  init(_ shape: jsShape) {
    offset = Position(shape.offset.x, shape.offset.y)
    blocks = BlockBuffer(buffer: UnsafeBufferPointer(start: shape.blocks, count: Shape.blockAmount))
  }
  
  var offsetedBlocks: [Block] {
    return blocks.map { (block) -> Block in return block.offseted(offset) }
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
  
  private var blockBuffer: UnsafeMutableBufferPointer<jsBlock> {
    return pointer.withMemoryRebound(to: jsBlock.self, capacity: 1, { (pointer) -> UnsafeMutableBufferPointer<jsBlock> in
      
      return UnsafeMutableBufferPointer(start: pointer, count: Board.blockAmount)
    })
  }
  
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
  
  var blocks: BlockBuffer {
    get {
      return BlockBuffer(buffer: pointer
        .withMemoryRebound(to: jsBlock.self, capacity: 1) { (pointer) -> UnsafeBufferPointer<jsBlock> in
          
          return UnsafeBufferPointer(start: pointer, count: Board.blockAmount)
      })
    } set {
      newValue.enumerated().forEach { (offset, block) in blockBuffer[offset] = block.raw }
    }
  }
  
  var rows: [BlockBuffer] {
    return (0..<Board.rowAmount).map { (i) -> BlockBuffer in
      
      return BlockBuffer(buffer: pointer
        .withMemoryRebound(to: jsRow.self, capacity: Board.rowAmount) { (pointer) -> UnsafeBufferPointer<jsBlock> in
          
          return UnsafeBufferPointer(start: pointer.advanced(by: i).withMemoryRebound(to: jsBlock.self, capacity: 1, { return $0 }),
                                     count: Board.columnAmount)
      })
    }
  }
  
}


class GameCordinator {
  
  enum MoveStatus {
    
    case mute
    case success
    case score
    case merge
    case failure
    
    init?(_ status: jsMoveStatus) {
      switch status {
      case jsMoveStatusMute:    self = .mute
      case jsMoveStatusSuccess: self = .success
      case jsMoveStatusScore:   self = .score
      case jsMoveStatusMerge:   self = .merge
      case jsMoveStatusFailure: self = .failure
      default:
        return nil
      }
    }
    
  }
  
  private var rawBoard: jsBoard
  private var rawShape: jsShape
  private var rawNextShape: jsShape
  
  var board: Board { return Board(board: &rawBoard) }
  var shape: Shape { return Shape(rawShape) }
  var nextShape: Shape { return Shape(rawNextShape) }
  
  init() {
    rawBoard = js_empty_board()
    rawShape = js_rand_shape()
    rawNextShape = js_rand_shape()
  }
  
  private init(rawBoard: jsBoard, rawShape: jsShape, rawNextShape: jsShape) {
    self.rawBoard = rawBoard
    self.rawShape = rawShape
    self.rawNextShape = rawNextShape
  }
  
  func popShape() {
    rawShape = rawNextShape
    rawNextShape = js_rand_shape()
  }
  
  func mergeShape() {
    js_merge(&rawBoard, &rawShape)
  }
  
  func resetGame(_ handler: () -> Void) {
    rawBoard = js_empty_board()
    handler()
  }
  
  struct TranslationResult {
    
    private let result: jsTranslationResult
    
    init(_ result: jsTranslationResult) { self.result = result }
    
    var status: MoveStatus { return MoveStatus(result.status)! }
    var newPosition: Position { return Position(result.new_position) }
    var offset: Position { return Position(result.offset) }
    
  }
  
  private func translateShape<Result>(vector: jsVec2i, handler: (TranslationResult) -> Result) -> (shape: jsShape, result: Result) {
    var result = js_translate_result(&rawShape, &rawBoard, vector)
    let handlerResult = handler(TranslationResult(result))
    
    guard TranslationResult(result).status == .merge else { return (js_translate_shape(&rawShape, &result), handlerResult) }
    
    mergeShape()
    popShape()
    
    return (rawShape, handlerResult)
  }
  
  func translateShapeDown<Result>(_ handler: (TranslationResult) -> Result) -> Result {
    let (shape, result) = translateShape(vector: jsVec2i(x: 0, y: -1), handler: handler)
    rawShape = shape
    return result
  }
  
  func translateShapeLeft<Result>(_ handler: (TranslationResult) -> Result) -> Result {
    let (shape, result) = translateShape(vector: jsVec2i(x: -1, y: 0), handler: handler)
    rawShape = shape
    return result
  }
  
  func translateShapeRight<Result>(_ handler: (TranslationResult) -> Result) -> Result {
    let (shape, result) = translateShape(vector: jsVec2i(x: 1, y: 0), handler: handler)
    rawShape = shape
    return result
  }
  
  struct RotationResult {
    
    private let result: jsRotationResult
    
    init(_ result: jsRotationResult) { self.result = result }
    
    var status: MoveStatus { return MoveStatus(result.status)! }
    
  }
  
  func rotateShapeClockwise<Result>(_ handler: (RotationResult) -> Result) -> Result {
    var result = js_rotate_result(&rawShape, &rawBoard, jsRotationClockwise)
    let handlerResult = handler(RotationResult(result))
    rawShape = js_rotate_shape(&rawShape, &result)
    
    return handlerResult
  }
  
  func rotateShapeCounterClockwise(_ handler: (RotationResult) -> Void) {
    var result = js_rotate_result(&rawShape, &rawBoard, jsRotationCounterClockwise)
    handler(RotationResult(result))
    rawShape = js_rotate_shape(&rawShape, &result)
  }
  
}

