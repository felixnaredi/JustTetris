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
    var didMerge: Bool { return raw.did_merge }
    
    private var indicies: [Int32] {
      return [
        raw.merge.indicies.0,
        raw.merge.indicies.1,
        raw.merge.indicies.2,
        raw.merge.indicies.3,
      ]
    }
    
    var clearedRows: [Int] {
      return (indicies
        .prefix(Int(raw.merge.rows_cleared)) as ArraySlice<Int32>)
        .map { Int($0) }
    }

    
    var oldShape: Shape { return Shape(js_result_old_shape(raw)) }
    var newShape: Shape { return Shape(js_result_new_shape(raw)) }
    
    fileprivate func clearRows(of board: UnsafeMutablePointer<jsBoard>) {
      js_clear_rows(
        board,
        indicies
          .withUnsafeBytes { (buffer) -> UnsafePointer<Int32> in
            return buffer.baseAddress!.assumingMemoryBound(to: Int32.self)
        },
        raw.merge.rows_cleared
      )
    }
    
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
    
    func increment(timer: Timer) -> Timer {
      return Timer(rawRuleset.increment_timer(timer.raw))
    }
    
    func timer(_ timer: Timer, for result: Result) -> Timer {
      return Timer(rawRuleset.timer_for_result(timer.raw, result.raw))
    }
    
    func duration(for level: Float) -> Int {
      return Int(rawRuleset.timer_force_down_for_level(level))
    }
    
    static var standard: Ruleset { return Ruleset(js_standard_ruleset()) }
    
  }
  
  struct Timer {
    
    let raw: jsTimer
    
    var forceDownDidTrigger: Bool { return raw.force_down_did_trigger }
    
    init(_ timer: jsTimer) { raw = timer }
    
    init(forLevel level: Float, with ruleset: Ruleset) {
      raw = jsTimer(time: 0,
                    force_down_time: ruleset.duration(for: level),
                    force_down_duration: Int32(ruleset.duration(for: level)),
                    force_down_did_trigger: false)
    }
    
    func increment(with ruleset: Ruleset) -> Timer {
      return ruleset.increment(timer: self)
    }
    
    func tick(with result: Result, ruleset: Ruleset) -> Timer {
      return ruleset.timer(self, for: result)
    }
    
    func withForceDownDuration(_ duration: Int) -> Timer {
      return Timer(jsTimer(time: raw.time,
                           force_down_time: raw.time + duration,
                           force_down_duration: Int32(duration),
                           force_down_did_trigger: false))
    }
    
  }

  private var rawBoard: jsBoard
  private var rawShape: jsShape
  private var rawNextShape: jsShape
  private var mutableLevel: Float = 0.0
  private var mutableScore: Float = 0.0
  private var mutableRowsCleared = 0
  
  private let ruleset = Ruleset.standard
  private var timer = Timer(forLevel: 0, with: Ruleset.standard)

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
  
  func clearRows(with result: Result) { result.clearRows(of: &rawBoard) }
  
  func incrementTimer() -> Result? {
    timer = ruleset.increment(timer: timer)
    guard timer.forceDownDidTrigger else { return nil }
    return translateShapeDown(user: false)
  }
  
  private func resetTimer() { timer = Timer(forLevel: 0, with: ruleset) }

  private func emptyBoard() { rawBoard = js_empty_board() }
  
  func reset() {
    popShape()
    popShape()
    emptyBoard()
    resetTimer()
    mutableScore = 0
    mutableLevel = 0
    mutableRowsCleared = 0
  }
  
  private func translate(vector: jsVec2i, user: Bool) -> Result {
    let result = Result(raw: js_translate_shape(&rawShape, &rawBoard, vector, user))
    let oldLevel = Int(level)
    
    mutableLevel += ruleset.levelIncrement(for: level, result: result)
    if Int(level) > oldLevel {
      timer = timer.withForceDownDuration(ruleset.duration(for: level))
    }
    
    mutableScore += ruleset.score(for: result) * ruleset.scoreMultiplier(for: level)
    mutableRowsCleared += result.clearedRows.count
    
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
