//
//  AppDelegate.swift
//  JustTetris
//
//  Created by Felix Naredi on 3/8/18.
//  Copyright © 2018 Felix Naredi. All rights reserved.
//

import Cocoa


@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
  
  let windowController = NSWindowController(windowNibName: NSNib.Name("Window"))
  
  func applicationDidFinishLaunching(_ notification: Notification) {
    windowController.window?.makeKeyAndOrderFront(self)
  }

}

