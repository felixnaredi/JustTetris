//
//  Tetris.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/8/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//
// 1
// 1
// 2Fibonacci
// 6
// 30
// 1
// 1
// 2
// 6
// 30 Fibonacci
// 240
// 3120
// 65520
// 1
// 1
// 2
// 6
// 30
// 240
// 3120
// 65520   Fibonacci
// 2227680
// 122522400
// 10904493600
// 1570247078400
// 365867569267200
// 1
// 1
// 2
// 6
// 30
// 240
// 3120
// 65520
// 2227680
// 122522400
// 10904493600
// 1570247078400
// 365867569267200         Fibonacci
// 137932073613734400
// 84138564904377984000
// 83044763560621070208000
// 132622487406311849122176000
// 342696507457909818131702784000
// 1432814097681520949608649339904000
// 9692987370815489224102512784450560000
// 106099439760946345047026104938595829760000


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
  
  struct TranslationResult {
    
    private let result: jsTranslationResult
    
    init(_ result: jsTranslationResult) { self.result = result }
    
    var status: MoveStatus { return MoveStatus(result.status)! }
    var newPosition: Position { return Position(result.new_position) }
    var offset: Position { return Position(result.offset) }
    
  }
  
  private func translateShape(vector: jsVec2i, handler: (TranslationResult) -> Void) -> jsShape {
    var result = js_translate_result(&rawShape, &rawBoard, vector)
    handler(TranslationResult(result))
    
    guard TranslationResult(result).status == .merge else { return js_translate_shape(&rawShape, &result) }
    
    mergeShape()
    popShape()
    
    return rawShape
  }
  
  func translateShapeDown(_ handler: (TranslationResult) -> Void) {
    rawShape = translateShape(vector: jsVec2i(x: 0, y: -1), handler: handler)
  }
  
  func translateShapeLeft(_ handler: (TranslationResult) -> Void) {
    rawShape = translateShape(vector: jsVec2i(x: -1, y: 0), handler: handler)
  }
  
  func translateShapeRight(_ handler: (TranslationResult) -> Void) {
    rawShape = translateShape(vector: jsVec2i(x: 1, y: 0), handler: handler)
  }
  
  struct RotationResult {
    
    private let result: jsRotationResult
    
    init(_ result: jsRotationResult) { self.result = result }
    
    var status: MoveStatus { return MoveStatus(result.status)! }
    
  }
  
  func rotateShapeClockwise(_ handler: (RotationResult) -> Void) {
    var result = js_rotate_result(&rawShape, &rawBoard, jsRotationClockwise)
    handler(RotationResult(result))
    rawShape = js_rotate_shape(&rawShape, &result)
  }
  
  func rotateShapeCounterClockwise(_ handler: (RotationResult) -> Void) {
    var result = js_rotate_result(&rawShape, &rawBoard, jsRotationCounterClockwise)
    handler(RotationResult(result))
    rawShape = js_rotate_shape(&rawShape, &result)
  }
  
}

