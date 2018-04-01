//
//  GameCordinator.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/31/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import Foundation


class GameCordinator {
  
  enum MoveStatus {
    
    case mute
    case success
    case merge
    case failure
    
    init?(_ status: jsMoveStatus) {
      switch status {
      case jsMoveStatusMute:    self = .mute
      case jsMoveStatusSuccess: self = .success
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
  
  func emptyBoard() { rawBoard = js_empty_board() }
  
  func mergeShape() {
    js_merge(&rawBoard, &rawShape)
  }
  
  struct ClearRowsResult {
    
    private let result: jsClearRowsResult
    
    init(_ result: jsClearRowsResult) { self.result = result }
    
    var amount: Int { return Int(result.count) }
    
    var indicies: [Int] {
      return [result.indicies.0,
              result.indicies.1,
              result.indicies.2,
              result.indicies.3,
              ].prefix(amount)
        .map { (n) -> Int in Int(n) }
    }
    
    var score: Float {
      var mutableResult = result
      return Float(js_clear_rows_score(&mutableResult))
    }
    
  }
  
  func clearRows<A, B>(boardWillChange: (ClearRowsResult) -> A,
                       boardDidChange: (ClearRowsResult) -> B) ->
    (boardWillChange: A?, boardDidChange: B?) {
      
      var rawResult = js_clear_rows_result(&rawBoard)
      let result = ClearRowsResult(rawResult)
      
      let boardWillChangeResult = boardWillChange(result)
      js_clear_board_rows(&rawBoard, &rawResult)
      
      return (boardWillChangeResult, boardDidChange(result))
  }
  
  
  struct TranslationResult {
    
    private let result: jsTranslationResult
    
    init(_ result: jsTranslationResult) { self.result = result }
    
    var status: MoveStatus { return MoveStatus(result.status)! }
    var newPosition: Position { return Position(result.new_position) }
    var offset: Position { return Position(result.offset) }
    var score: Float {
      var mutableResult = result
      return Float(js_translate_score(&mutableResult))
    }
    
  }
  
  private func translateShape<A, B>(_ vector: jsVec2i,
                                    _ shapeWillChange: (TranslationResult) -> A?,
                                    _ shapeDidChange: (TranslationResult) -> B?) ->
    (shapeWillChange: A?, shapeDidChange: B?) {
    
    var rawResult = js_translate_result(&rawShape, &rawBoard, vector)
    let result = TranslationResult(rawResult)
    
    let ws = shapeWillChange(result)
    rawShape = js_translate_shape(&rawShape, &rawResult)
    
    return (ws, shapeDidChange(result))
  }  
  
  func translateShapeDown<A, B>(shapeWillChange: (TranslationResult) -> A,
                                 shapeDidChange: (TranslationResult) -> B) ->
    (shapeWillChange: A?, shapeDidChange: B?) {
      
      return translateShape(jsVec2i(x: 0, y: -1), shapeWillChange, shapeDidChange)
  }
  
  func translateShapeLeft<A, B>(shapeWillChange: (TranslationResult) -> A,
                                 shapeDidChange: (TranslationResult) -> B) ->
    (shapeWillChange: A?, shapeDidChange: B?) {
      
      return translateShape(jsVec2i(x: -1, y: 0), shapeWillChange, shapeDidChange)
  }
  
  func translateShapeRight<A, B>(shapeWillChange: (TranslationResult) -> A,
                                 shapeDidChange: (TranslationResult) -> B) ->
    (shapeWillChange: A?, shapeDidChange: B?) {
    
    return translateShape(jsVec2i(x: 1, y: 0), shapeWillChange, shapeDidChange)
  }
  
  
  struct RotationResult {
    
    private let result: jsRotationResult
    
    init(_ result: jsRotationResult) { self.result = result }
    
    var status: MoveStatus { return MoveStatus(result.status)! }
    
  }
  
  private func rotateShape<A, B>(_ direction: jsRotation,
                                 _ shapeWillChange: (RotationResult) -> A?,
                                 _ shapeDidChange: (RotationResult) -> B?) ->
    (shapeWillChange: A?, shapeDidChange: B?) {
    
      var rawResult = js_rotate_result(&rawShape, &rawBoard, direction)
      let result = RotationResult(rawResult)
      
      let shapeWillChangeResult = shapeWillChange(result)
      rawShape = js_rotate_shape(&rawShape, &rawResult)
      
      return (shapeWillChangeResult, shapeDidChange(result))
  }
  
  func rotateShapeClockwise<A, B>(shapeWillChange: (RotationResult) -> A?,
                                  shapeDidChange: (RotationResult) -> B?) ->
    (shapeWillChange: A?, shapeDidChange: B?) {
      
      return rotateShape(jsRotationClockwise, shapeWillChange, shapeDidChange)
  }
  
  func rotateShapeCounterClockwise<A, B>(shapeWillChange: (RotationResult) -> A?,
                                         shapeDidChange: (RotationResult) -> B?) ->
    (shapeWillChange: A?, shapeDidChange: B?) {
      
      return rotateShape(jsRotationCounterClockwise, shapeWillChange, shapeDidChange)
  }
  
}

