//
//  ViewController.swift
//  JustTetris
//
//  Created by Felix Naredi on 4/17/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import Cocoa


class ViewController: NSViewController {
  
  @IBOutlet weak var shapeView: ShapeView!
  @IBOutlet weak var boardView: BoardView!
  @IBOutlet weak var gridViewDelegate: GridViewDelegate!
  
  @IBOutlet weak var rowLabel: NSTextField?
  @IBOutlet weak var scoreLabel: NSTextField?
  @IBOutlet weak var levelLabel: NSTextField?
  
  private func updateLabels() {
    rowLabel?.stringValue = "Rows: \(gameCordinator.rowsCleared)"
    scoreLabel?.stringValue = "Score: \(Int(gameCordinator.score))"
    levelLabel?.stringValue = "Level: \(Int(gameCordinator.level))"
  }
  
  let gameCordinator = GameCordinator()
  var timer: Timer?
  
  private func handleResult(_ result: GameCordinator.Result) {
    if result.didMerge {
      gameCordinator.clearRows(with: result)
      gameCordinator.popShape()
      shapeView.setBlocks(
        with: gameCordinator.nextShape.blocks
      )
      shapeView.needsDisplay = true
    }
    
    boardView.setBlocks(
      with: gameCordinator.board.blocks +
            gameCordinator.shape.offsetedBlocks
    )
    boardView.needsDisplay = true
    
    if result.gameOver {
      timer?.invalidate()
      timer = nil
    }
    
    updateLabels()
  }
  
  private func scheduleTimer() -> Timer {
    return Timer.scheduledTimer(
      withTimeInterval: 1.0 / 60.0,
      repeats: true,
      block: { _ in
        guard let result = self.gameCordinator.incrementTimer()
          else { return }
        self.handleResult(result)
    })
  }
  
  private func fall() -> GameCordinator.Result {
    let result = gameCordinator.translateShapeDown(user: true)
    guard !result.successfull else { return fall() }
    return result
  }
  
  private func result(for event: NSEvent)
    -> GameCordinator.Result?
  {
    switch event.keyCode {
    case 123:
      return gameCordinator.translateShapeLeft(user: true)
    case 124:
      return gameCordinator.translateShapeRight(user: true)
    case 125:
      return gameCordinator.translateShapeDown(user: true)
    case 126:
      return gameCordinator.rotateClockwise(user: true)
    case 49:
      return fall()
    case 35:
      if timer == nil { timer = scheduleTimer() }
      else {
        timer?.invalidate()
        timer = nil
      }
      return nil
    default:
      return nil
    }
  }
  
  override func viewDidLoad() {
    shapeView.delegate = gridViewDelegate
    shapeView.device = gridViewDelegate.device
    shapeView.enableSetNeedsDisplay = true
    shapeView.setBlocks(with: gameCordinator.nextShape.blocks)
    
    shapeView.mouseDownHandler = { _ in
      self.gameCordinator.popShape()
      self.shapeView
        .setBlocks(with: self.gameCordinator.nextShape.blocks)
      self.shapeView.needsDisplay = true
    }
    
    boardView.delegate = gridViewDelegate
    boardView.device = gridViewDelegate.device
    boardView.enableSetNeedsDisplay = true
    boardView.setBlocks(with: gameCordinator.shape.offsetedBlocks)
    
    boardView.keyDownHandler = { (event) in
      guard let result = self.result(for: event)
      else { return }
      self.handleResult(result)
    }
    
    timer = scheduleTimer()
    updateLabels()
  }
  
  @IBAction func newGameButtonPressed(_ sender: Any?) {
    gameCordinator.reset()
    
    boardView.setBlocks(
      with: gameCordinator.board.blocks +
            gameCordinator.shape.offsetedBlocks
    )
    boardView.needsDisplay = true
    
    shapeView.setBlocks(
      with: gameCordinator.nextShape.blocks
    )
    shapeView.needsDisplay = true
    
    updateLabels()
  }
  
  @IBAction func breakPointButtonPressed(_ sender: Any?) {
  
  }
  
}
