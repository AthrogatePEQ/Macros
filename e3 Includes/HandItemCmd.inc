|
| HandItemCmd.inc r4
|   Adds an eqbc chat command for directing bots to hand an item to your target (player or npc).
|   Run command to create alias:
|   /noparse /alias /handitem /bc HandItem ${Target.ID} |
|
|   Syntax for /handitem is: (Item name must be exact, can also be an item link)
|     /handitem ItemName|ItemLink
|
|   Example. I want all bots to hand Sliver of the High Temple to my target (which happens to be Tublik in KT).
|     /handitem Sliver of the High Temple
|
|   Can hand multiple items at once by separating them with |'s (mixed item links or names are fine)
|     /handitem Sliver of the High Temple|Splinter of the High Temple|Fragment of the High Temple
|
|   Can use inclusion args - same as above but only rogues turn in:
|     /handitem Sliver of the High Temple|Splinter of the High Temple|Fragment of the High Temple /only|rog
|
|   If a stackable item is specified, it will trade the first stack regardless of size. Multiple stacks can not be traded at once.
|

| Max time to move to targer
#Define MAX_MOVE_TIME 100

| Max initial distance from target allowed
#Define MAX_MOVE_DIST 200

| Max time to wait for a player to accept trade
#Define MAX_WAIT_TIME_PLAYER_TRADE 60s

|#Event HandItemCmd "[#1#(msg)] HandItem #2# | #3#"
|#Event HandItemCmd "<#1#> HandItem #2# | #3#"
#Event HandItemCmd "<#1#> HandItem #2# #3#"
Sub Event_HandItemCmd(line, chatSender, targetID, args)
|/bc Event_HandItemCmd(line, ${chatSender}, ${targetID}, ${args})
  /if (!(${NetBots[${chatSender}].ID} && ${NetBots[${chatSender}].InZone})) /return
  /if (!${Spawn[id ${targetID}].LineOfSight}) {
    /bc Spawn id ${targetID} is not in line of sight.
    /return
  }

  /if (${targetID}==${Me.ID}) /return
  /declare hicItemSlot string local
  /declare hicDontHaveItems string local
  /declare hicItemCount int local 0
  /declare hicCurrentItem int local 0
  /declare i int local 0
  
  /call #CreateVars string outer HICReturn ItemPart InclusionPart  
  /call #SplitInclusionArgs "${args}" ItemPart InclusionPart
  /call #UnlinkAllItems "${ItemPart}" HICReturn

  /call #verifyEvent "${chatSender}" "${InclusionPart}"
  /if (!${Macro.Return}) /return

  /keypress OPEN_INV_BAGS

  /for i 1 to ${Math.Calc[${HICReturn.Count[|]}+1]}
    /if (${Bool[${FindItem[=${HICReturn.Arg[${i},|]}].InvSlot.ID}]}) {
      /varset hicItemSlot ${hicItemSlot}${If[${i}>1,|,]}${FindItem[=${HICReturn.Arg[${i},|]}].InvSlot.ID}
    } else {
      /varset hicDontHaveItems ${hicDontHaveItems}${If[${hicDontHaveItems.Length},|,]}${HICReturn.Arg[${i},|]}
    }
  /next i
  /if (${hicDontHaveItems.Length}) {
    /bc I do not have the following items: ${hicDontHaveItems}
    /return
  }
  /varcalc hicItemCount ${HICReturn.Count[|]}+1
  /declare hicMoveTimer timer local 0
  /declare hicFailMessage string local
  /declare hicResumeNetFollow int local ${NetAdvPath.Status}
  /if (${hicResumeNetFollow}==1) /squelch /netfollow pause
  
  /squelch /stick off
  /squelch /moveto save
  /squelch /moveto 5
  /if (${Spawn[id ${targetID}].Distance} > MAX_MOVE_DIST) {
    /varset hicFailMessage Spawn id ${targetID} too far away. (Distance: ${Spawn[id ${targetID}].Distance})
    /goto :Failed
  }
  /if (${Spawn[id ${targetID}].Distance} > 14) {
    /squelch /moveto loc ${Spawn[id ${targetID}].Y} ${Spawn[id ${targetID}].X}
    /varset hicMoveTimer MAX_MOVE_TIME
    /delay 10 ${MoveTo.Moving}
    :MoveLoop
      /if (!${hicMoveTimer}) {
        /varset hicFailMessage I was unable to move to target id ${targetID}.
        /goto :Failed
      }
      /delay 5
      /if (${MoveTo.Stopped}) /goto :DoHandItem
    /goto :MoveLoop
  }
  :DoHandItem
    /varcalc hicCurrentItem ${hicCurrentItem}+1
    /squelch /target id ${targetID}
    /delay 10 ${Target.ID}
    /keypress OPEN_INV_BAGS
    /delay 5
    /shiftkey /itemnotify ${hicItemSlot.Arg[${hicCurrentItem},|]} leftmouseup
    /delay 20 ${Cursor.ID}
    /if (!${Cursor.ID}) {
      /varset hicFailMessage I can't pick up item slot ${hicItemSlot.Arg[${hicCurrentItem},|]}
      /goto :Failed
    }
    /click left target
    /delay 50 ${Window[GiveWnd].Open} || ${Window[TradeWnd].Open}
    /if (!${Window[GiveWnd].Open} && !${Window[TradeWnd].Open}) {
      /varset hicFailMessage I can't hand item to target id ${targetID}
      /goto :Failed
    } 
    /if (${hicCurrentItem}<${hicItemCount}) /goto :DoHandItem
    /if (${Window[GiveWnd].Open}) {
      /notify GiveWnd GVW_Give_Button leftmouseup
      /delay 20 !${Window[GiveWnd].Open}
    } else /if (${Window[TradeWnd].Open}) {
      /notify TradeWnd TRDW_Trade_Button leftmouseup
      /delay MAX_WAIT_TIME_PLAYER_TRADE !${Window[TradeWnd].Open}
    }
    /goto :Cleanup
  :Failed
    /bc ${hicFailMessage}
  :Cleanup
    /squelch /moveto off
    /squelch /moveto load
    /if (${Cursor.ID}) /autoinv
    /cleanup
    /if (${hicResumeNetFollow}==1) /squelch /netfollow resume
    /keypress CLOSE_INV_BAGS
/return

Sub HandItemCMD_Background_Events
  /doevents HandItemCmd
/return

Sub HandItemCMD_Aliases
    /noparse /alias /handitem /bc HandItem ${Target.ID}|
/return

Sub HandItemCMD_Setup
/return

Sub HandItemCMD_CharacterSettings
/return

Sub HandItemCMD_MacroSettings
/return