//
//  GameCordinator.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/31/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import Foundation

enum MoveStatus {

  case mute
  case success
  case merge
  case failure
  case gameOver

  init?(_ status: jsMoveStatus) {
    switch status {
    case jsMoveStatusMute:     self = .mute
    case jsMoveStatusSuccess:  self = .success
    case jsMoveStatusMerge:    self = .merge
    case jsMoveStatusFailure:  self = .failure
    case jsMoveStatusGameOver: self = .gameOver
    default:
      return nil
    }
  }

}

protocol MoveResult {

  associatedtype RawMoveResult

  var status: MoveStatus { get }
  var oldShape: Shape { get }
  var newShape: Shape { get }
  var score: Float { get }

  init(result: RawMoveResult, shape: jsShape)

}


class GameCordinator {

  private var rawBoard: jsBoard
  private var rawShape: jsShape
  private var rawNextShape: jsShape

  var board: Board { return Board(board: rawBoard) }
  var shape: Shape { return Shape(rawShape) }
  var nextShape: Shape { return Shape(rawNextShape) }

  init() {
    rawBoard = js_empty_board()
    rawShape = js_rand_shape()
    rawNextShape = js_rand_shape()
  }

  func popShape() {
    rawShape = rawNextShape
    rawNextShape = js_rand_shape()
  }

  func emptyBoard() { rawBoard = js_empty_board() }

  func mergeShape() {
    js_merge(&rawBoard, &rawShape)
  }

  struct ClearRowsResult {

    private let result: jsClearRowsResult

    init(_ result: jsClearRowsResult) { self.result = result }

    var count: Int { return Int(result.count) }

    var indicies: [Int] {
      return [result.indicies.0,
              result.indicies.1,
              result.indicies.2,
              result.indicies.3,
             ].prefix(count)
        .map { (n) -> Int in Int(n) }
    }

    var score: Float {
      var mutableResult = result
      return Float(js_clear_rows_score(&mutableResult))
    }

  }

  func clearRows<A, B>(
    boardWillChange: (ClearRowsResult) -> A,
    boardDidChange: (ClearRowsResult) -> B
    ) -> (
    boardWillChange: A?,
    boardDidChange: B?
    ) {
      var rawResult = js_clear_rows_result(&rawBoard)
      let result = ClearRowsResult(rawResult)

      let boardWillChangeResult = boardWillChange(result)
      js_clear_board_rows(&rawBoard, &rawResult)

      return (boardWillChangeResult, boardDidChange(result))
  }


  struct TranslationResult: MoveResult {

    private let result: jsTranslationResult
    private let shape: jsShape

    init(result: jsTranslationResult, shape: jsShape) {
      self.result = result
      self.shape = shape
    }

    var status: MoveStatus { return MoveStatus(result.status)! }

    var newShape: Shape {
      var mutableShape = shape
      var mutableResult = result
      return Shape(js_translate_shape(&mutableShape, &mutableResult))
    }

    var oldShape: Shape { return Shape(shape) }

    var score: Float {
      var mutableResult = result
      return Float(js_translate_score(&mutableResult))
    }

  }
  
  struct RotationResult: MoveResult {
    
    private let result: jsRotationResult
    private let shape: jsShape
    
    init(result: jsRotationResult, shape: jsShape) {
      self.result = result
      self.shape = shape
    }
    
    var status: MoveStatus { return MoveStatus(result.status)! }
    var oldShape: Shape { return Shape(shape) }
    var score: Float { return 0 }
    
    var newShape: Shape {
      var mutableShape = shape
      var mutableResult = result
      return Shape(js_rotate_shape(&mutableShape, &mutableResult))
    }
    
  }
  
  typealias MoveHandlerReturn<A, B> = (shapeWillChange: A?, shapeDidChange: B?)
  
  private func moveShape<Result: MoveResult, Direction, A, B>(
    _ getResult: (UnsafePointer<jsShape>, UnsafePointer<jsBoard>, Direction) -> Result.RawMoveResult,
    _ setResult: (UnsafePointer<jsShape>, UnsafePointer<Result.RawMoveResult>) -> jsShape,
    _ direction: Direction,
    _ shapeWillChange: (Result) -> A?,
    _ shapeDidChange: (Result) -> B?
    ) -> MoveHandlerReturn<A, B> {
      var rawResult = getResult(&rawShape, &rawBoard, direction)
      let result = Result(result: rawResult, shape: rawShape)
      
      let shapeWillChangeReturn = shapeWillChange(result)
      rawShape = setResult(&rawShape, &rawResult)
      return (shapeWillChangeReturn, shapeDidChange(result))
  }

  private func translateShape<Result: MoveResult, A, B>(
    _ vector: jsVec2i,
    _ shapeWillChange: (Result) -> A?,
    _ shapeDidChange: (Result) -> B?
    ) -> MoveHandlerReturn<A, B>
    where Result.RawMoveResult == jsTranslationResult
  {
    return moveShape(js_translate_result, js_translate_shape, vector, shapeWillChange, shapeDidChange)
  }
  
  private func rotateShape<Result: MoveResult, A, B>(
    _ direction: jsRotation,
    _ shapeWillChange: (Result) -> A?,
    _ shapeDidChange: (Result) -> B?
    ) -> MoveHandlerReturn<A, B>
    where Result.RawMoveResult == jsRotationResult
  {
    return moveShape(js_rotate_result, js_rotate_shape, direction, shapeWillChange, shapeDidChange)
  }

  func translateShapeDown<A, B>(
    shapeWillChange: (TranslationResult) -> A?,
    shapeDidChange: (TranslationResult) -> B?
    ) -> MoveHandlerReturn<A, B> {
      return translateShape(jsVec2i(x: 0, y: -1), shapeWillChange, shapeDidChange)
  }

  func translateShapeLeft<A, B>(
    shapeWillChange: (TranslationResult) -> A?,
    shapeDidChange: (TranslationResult) -> B?
    ) -> MoveHandlerReturn<A, B> {
      return translateShape(jsVec2i(x: -1, y: 0), shapeWillChange, shapeDidChange)
  }

  func translateShapeRight<A, B>(
    shapeWillChange: (TranslationResult) -> A,
    shapeDidChange: (TranslationResult) -> B?
    ) -> MoveHandlerReturn<A, B> {
      return translateShape(jsVec2i(x: 1, y: 0), shapeWillChange, shapeDidChange)
  }

  func rotateShapeClockwise<A, B>(
    shapeWillChange: (RotationResult) -> A?,
    shapeDidChange: (RotationResult) -> B?
    ) -> MoveHandlerReturn<A, B> {
      return rotateShape(jsRotationClockwise, shapeWillChange, shapeDidChange)
  }

  func rotateShapeCounterClockwise<A, B>(
    shapeWillChange: (RotationResult) -> A?,
    shapeDidChange: (RotationResult) -> B?
    ) -> MoveHandlerReturn<A, B> {
      return rotateShape(jsRotationCounterClockwise, shapeWillChange, shapeDidChange)
  }

}
