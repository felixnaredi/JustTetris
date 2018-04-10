//
//  GameCordinator.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/31/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import Foundation


class GameCordinator {
  
  struct Result {
    
    let raw: jsResult
    
    var isMuteAction: Bool { return raw.mute_action }
    var userAction: Bool { return raw.user_action }
    var gameOver: Bool { return raw.game_over }
    var successfull: Bool { return raw.successfull }
    var didMerge: Bool { return raw.merge.should_merge }
    
    var rowsCleared: Int { return Int(raw.merge.rows_cleared) }
    
    var oldShape: Shape { return Shape(js_result_old_shape(raw)) }
    var newShape: Shape { return Shape(js_result_new_shape(raw)) }
    
  }
  
  struct Ruleset {
    
    private let rawRuleset: jsRuleset
    
    init(_ ruleset: jsRuleset) { rawRuleset = ruleset }
    
    func scoreMultiplier(for level: Float) -> Float {
      return rawRuleset.level_score_multiplier(level)
    }
    
    func score(for result: Result) -> Float {
      return [
        rawRuleset.score_for_translation(result.raw),
        rawRuleset.score_for_clear(result.raw)
        
        ].reduce(0) { (sum, element) -> Float in return sum + element }
    }
    
    func levelIncrement(for level: Float, result: Result) -> Float {
      return rawRuleset.level_increment_for_clear(level, result.raw)
    }
    
    static var standard: Ruleset { return Ruleset(js_standard_ruleset()) }
    
  }

  private var rawBoard: jsBoard
  private var rawShape: jsShape
  private var rawNextShape: jsShape
  private var mutableLevel: Float = 0.0
  private var mutableScore: Float = 0.0
  private var mutableRowsCleared = 0
  
  private let ruleset = Ruleset(js_standard_ruleset())

  var board: Board { return Board(board: rawBoard) }
  var shape: Shape { return Shape(rawShape) }
  var nextShape: Shape { return Shape(rawNextShape) }
  var level: Float { return mutableLevel }
  var score: Float { return mutableScore }
  var rowsCleared: Int { return mutableRowsCleared }

  init() {
    rawBoard = js_empty_board()
    rawShape = js_rand_shape()
    rawNextShape = js_rand_shape()
  }

  func popShape() {
    rawShape = rawNextShape
    rawNextShape = js_rand_shape()
  }

  private func emptyBoard() { rawBoard = js_empty_board() }
  
  func reset() {
    popShape()
    popShape()
    emptyBoard()
    mutableScore = 0
    mutableLevel = 0
    mutableRowsCleared = 0
  }
  
  private func translate(vector: jsVec2i, user: Bool) -> Result {
    let result = Result(raw: js_translate_shape(&rawShape, &rawBoard, vector, user))
    
    mutableLevel += ruleset.levelIncrement(for: level, result: result)
    mutableScore += ruleset.score(for: result) * ruleset.scoreMultiplier(for: level)
    mutableRowsCleared += result.rowsCleared
    
    return result
  }
  
  func translateShapeDown(user: Bool) -> Result {
    return translate(vector: jsVec2i(x: 0, y: -1), user: user)
  }
  
  func translateShapeLeft(user: Bool) -> Result {
    return translate(vector: jsVec2i(x: -1, y: 0), user: user)
  }
  
  func translateShapeRight(user: Bool) -> Result {
    return translate(vector: jsVec2i(x: 1, y: 0), user: user)
  }
  
  private func rotate(direction: jsRotate, user: Bool) -> Result {
    return Result(raw: js_rotate_shape(&rawShape, &rawBoard, direction, user))
  }
  
  func rotateClockwise(user: Bool) -> Result{
    return rotate(direction: jsRotateClockwise, user: user)
  }

}
