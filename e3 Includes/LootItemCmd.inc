|
| LootItemCmd.inc r5
|   Adds an eqbc chat command for looting a specific item off target corpse. Another command can equip the last item looted.
|   Run these commands to create aliases:
|   /noparse /alias /lootitem /bc LootItem ${Target.ID}
|   /alias /stoploot /bc StopLoot
|   /alias /equipthat /bc EquipThat
|
|   Using /stoploot before the item is actually looted will cancel the looting action.
|   Syntax for /lootitem is: (Item name must be exact, can also be an item link)
|     /lootitem CharName ItemName|ItemLink
|
|   Example. I want Mehcanic to loot Chaotic Black Scimitar off of my target corpse.
|     /lootitem Mehcanic Chaotic Black Scimitar
|
|   To equip the last item looted, use /equipthat
|   Example. Mehcanic just looted Chaotic Black Scimitar. I want him to equip it in offhand slot:
|     /equipthat Mehcanic offhand
|   
|   Example. Mehcanic just looted a ring. I want him to equip it in right finger slot:
|     /equipthat Mehcanic right
|     /equipthat Mehcanic right finger
|     /equipthat Mehcanic ring right
|
|   Example. Mehcanic just looted a helm. I want him to equip it.
|     /equipthat Mehcanic
|
|   If the destination slot is ambiguous (for example, can be equipped in finger AND ear slots), it must be stated in the command.
|
| Requires:
|#include vyslib.inc

#Event LootItemCmd "<#1#> LootItem #2# |${Me.Name}| #3#"
#Event LootItemCmd "<#1#> LootItem #2# needed #3#"
#Event StopLoot "<#1#> StopLoot"
#Event licEquipThat "<#1#> EquipThat |${Me.Name}|"
#Event licEquipThat "<#1#> EquipThat |${Me.Name}| #2#"

| Max time to move to corpse
#Define MAX_MOVE_TIME 200

| Time to wait to open corpse (if someone else has it open etc)
#Define WAIT_TIME_TO_LOOT 200

| Max initial distance from corpse allowed
#Define MAX_MOVE_DIST 200

Sub LootItemCMD_Background_Events
  /doevents LootItemCmd
  /doevents StopLoot
  /doevents licEquipThat
/return

Sub LootItemCMD_Aliases
  /noparse /alias /lootitem /bc LootItem ${Target.ID}
  /alias /stoploot /bc StopLoot
  /alias /equipthat /bc EquipThat
/return

Sub LootItemCMD_Setup
/return

Sub LootItemCMD_CharacterSettings
/return

Sub LootItemCMD_MacroSettings
/return

Sub Event_LootItemCmd(line, chatSender, targetID, args)
  /if (!(${NetBots[${chatSender}].ID} && ${NetBots[${chatSender}].InZone})) /return


  |/declare useItemByID int outer 0
  |/call #RequirePlugin MQ2Fusion
  |/varset useItemByID ${Macro.Return}
  |/if (${useItemByID}) {
  |  /call #RequirePlugin MQ2Format
  |  /varset useItemByID ${Macro.Return}
  |}

  /call #CreateVars string outer LICReturn ItemPart InclusionPart
  /call #SplitInclusionArgs "${args}" ItemPart InclusionPart

  /call #verifyEvent "${chatSender}" "${InclusionPart}"
  /if (!${Macro.Return}) /return

  /varset args ${ItemPart}

  /if (${line.Find[ needed ]}) {
    /if (${FindItemCount[=${args}]} || ${FindItemBankCount[=${args}]}) /return
    /call #StripItemLink "${args}" LICReturn
    |/if (${useItemByID}) {
    |  /if (${FindItemByID[${HexToInt[${LICReturn.Left[6]}]}]} > 0) /return
    |} else {
    |  /if (${FindItemCount[=${LICReturn}]} || ${FindItemBankCount[=${LICReturn}]}) /return
    |}
    /if (${FindItemCount[=${LICReturn}]} || ${FindItemBankCount[=${LICReturn}]}) /return
    |/actionrequest Loot ID ${targetID}
    |/delay 50 ${ActionLock.Request}
    |/if (!${ActionLock.Proceed}) {
    |  /return
    |}
    /declare LootItemActionLocked string local Loot ID ${targetID}
  }
  /if (!${Spawn[id ${targetID}].LineOfSight}) {
    /bc Spawn id ${targetID} is not in line of sight.
    /return
  }

  /if (!${Defined[licStopLoot]}) /declare licStopLoot int outer 0
  /if (!${Defined[licLastLooted]}) /declare licLastLooted string outer
  /varset licStopLoot 0
  /declare licMoveTimer timer local 0
  /declare licLootTimer timer local 0
  /declare licFailMessage string local
  /declare licLootTotal int local -1
	/declare licLootTotalTimer timer local 5s
  /declare licResumeNetFollow int local ${NetAdvPath.Status}
  /if (${licResumeNetFollow}==1) /squelch /netfollow pause
  /bc looting...

  /squelch /stick off
  /squelch /moveto save
  /squelch /moveto 5
  /if (${Spawn[id ${targetID}].Distance} > MAX_MOVE_DIST) {
    /varset licFailMessage Spawn id ${targetID} too far away. (Distance: ${Spawn[id ${targetID}].Distance})
    /goto :Failed
  }
  /if (${Spawn[id ${targetID}].Distance} > 14) {
    /squelch /moveto loc ${Spawn[id ${targetID}].Y} ${Spawn[id ${targetID}].X} uw
    /varset licMoveTimer MAX_MOVE_TIME
    /delay 10 ${MoveTo.Moving}
    :MoveLoop
      /doevents licStopLoot
      /if (${licStopLoot}) {
        /varset licFailMessage Cancelling loot routine...
        /goto :Failed
      }
      /if (!${licMoveTimer}) {
        /varset licFailMessage I was unable to move to target id ${targetID}.
        /goto :Failed
      }
      /delay 5
      /if (${MoveTo.Stopped}) /goto :DoLoot
    /goto :MoveLoop
  }
  :DoLoot
    /varset licLootTimer WAIT_TIME_TO_LOOT
    :RetryLoot
    /squelch /target id ${targetID}
    /delay 10 ${Target.ID}
    /loot
    /delay 10 ${Corpse.Open}
    /delay 10
    /doevents StopLoot
    /if (${licStopLoot}) {
      /varset licFailMessage Cancelling loot routine...
      /goto :Failed
    }
    /if (!${Corpse.Open}) {
      /if (!${licLootTimer}) {
        /varset licFailMessage Can't open corpse id ${targetID}
        /goto :Failed
      } else {
        /goto :RetryLoot
      }
    }
    :LootLag
    /if (${licLootTotal}!=${Corpse.Items}) {
      /varset licLootTotal ${Corpse.Items}
      /delay 5
      /if (${licLootTotalTimer}) /goto :LootLag
    }
    /doevents StopLoot
    /if (${licStopLoot}) {
      /varset licFailMessage Cancelling loot routine...
      /goto :Failed
    }
    | If item not found, assume it is an item link and try again

    /call #StripItemLink "${args}" LICReturn
    /varset args ${LICReturn}
    |/bc args: ${args} LICReturn: ${LICReturn} slot: ${Corpse.Item[=${args}].InvSlot.ID}
    /if (${Bool[${Corpse.Item[=${args}].InvSlot.ID}]}) {
      /declare licItemSlot int local ${Corpse.Item[=${args}].InvSlot.ID}
      | Sanity check to make absolutely sure we are looting the right thing.
      |/if (${licItemSlot} && ${InvSlot[${licItemSlot}].Item.Name.Equal[${args}]}) {
      /if (${licItemSlot} ) {
        /itemnotify ${licItemSlot} rightmouseup
      } else {
        /varset licFailMessage Found [${args}] on corpse but something went wrong.
        /goto :Failed
      }
    } else {
      /varset licFailMessage Item [${args}] not found on corpse.
      /goto :Failed
    }
  /varset licLastLooted ${args}
  /goto :Cleanup
  :Failed
    /bc ${licFailMessage}
  :Cleanup
    /doevents flush StopLoot
    /squelch /moveto off
    /if (${Window[LootWnd]}) {
      /notify LootWnd DoneButton leftmouseup
      /bc Looted: ${args}
    }
    /squelch /moveto load
    /if (${licResumeNetFollow}==1) /squelch /netfollow resume
    |/if (${Defined[LootItemActionLocked]}) /actionrelease ${LootItemActionLocked}
/return

Sub Event_StopLoot(line, chatSender)
  /varset licStopLoot 1
/return


Sub Event_licEquipThat(line, chatSender, args)
  /if (!${Defined[licLastLooted]} || !${licLastLooted.Length}) /return
  /if (!${Plugin[MQ2Exchange].Name.Length}) /plugin MQ2Exchange noauto
    /if (!${Plugin[MQ2Exchange].Name.Length}) {
      /bc I can't load MQ2Exchange.
      /return
    }
  /if (!${Defined[licDestSlot]}) /declare licDestSlot string outer
  /if (!${Defined[licSlotData]}) /declare licSlotData string outer
  /if (!${Defined[licPossibleSlots]}) /declare licPossibleSlots string outer
  /varset licDestSlot
  /varset licSlotData
  /varset licPossibleSlots
  /if (!${FindItemCount[${licLastLooted}]}) {
    /bc I do not have item ${licLastLooted}
    /return
  }
  /if (${FindItem[${licLastLooted}].WornSlots}>1) {
    /call licParseEquipArgs "${args}"
    /if (!${licDestSlot.Length} && ${FindItem[${licLastLooted}].WornSlots}<=2) {
      /call licGetEquipableSlots "${licLastLooted}" licPossibleSlots
      /call licChooseSlot "${licPossibleSlots}" licDestSlot
    }
    /call licResolveSlotName "${licDestSlot}" licDestSlot
  } else {
    /varset licDestSlot ${FindItem[${licLastLooted}].WornSlot[1].Name}
  }
  
  /if (${licDestSlot.Length}) {
    /if (!${FindItem[${licLastLooted}].WornSlot[${licDestSlot}]}) {
      /bc Can't equip ${licLastLooted} in slot ${licDestSlot}
    } else {
      /bc Equipping ${FindItem[${licLastLooted}].Name} in slot ${licDestSlot}
      /exchange "${licLastLooted}" ${licDestSlot}
    }
  } else {
    /call licGetEquipableSlots "${licLastLooted}" licPossibleSlots
    /bc Can't equip ${licLastLooted} in ambiguous slot. Possible slots: ${licPossibleSlots}
  }
    
/return

Sub licGetEquipableSlots(itemName, destVar)
  /declare i int local 0
  /for i 1 to ${FindItem[${itemName}].WornSlots}
    /if (${i}==1) {
      /varset ${destVar} ${FindItem[${itemName}].WornSlot[${i}].Name}
    } else {
      /varset ${destVar} ${${destVar}}|${FindItem[${itemName}].WornSlot[${i}].Name}
    }
  /next i
/return

Sub licChooseSlot(args, destVar)
  /if (${args.Find[ear]}) {
    /varset ${destVar} ear
  } else /if (${args.Find[wrist]}) {
    /varset ${destVar} wrist
  } else /if (${args.Find[finger]}) {
    /varset ${destVar} finger
  } else /if (${args.Find[primary]}) {
    /varset ${destVar} primary
  }
/return

Sub licParseEquipArgs(args)
  /declare argCount int local ${Math.Calc[1+${args.Count[ ]}]}
  /declare currentArg string local 
  /declare nextArg int local 0
  :NextArg
  /varcalc nextArg ${nextArg}+1
  /varset currentArg ${args.Arg[${nextArg}, ]}
  /if (${Select[${currentArg},left,right]}) {
    /varset licSlotData ${currentArg}
  } else {
    /varset licDestSlot ${currentArg}
  }
  /if (${nextArg}<${argCount}) /goto :NextArg
/return

Sub licResolveSlotName(ref, destVar)
  /if (${Select[${ref},charm,leftear,head,face,rightear,neck,shoulder,arms,back,leftwrist,rightwrist,ranged,hands,mainhand,offhand,leftfinger,rightfinger,chest,legs,feet,waist,powersource,ammo]}) {
    /varset ${destVar} ${ref}
    /return
  }
  /varset ${destVar}
  /if (${Select[${licSlotData},left,right]}) {
    /if (${Select[${ref},ear,ears,earrings,earring]}) {
      /varset ${destVar} ${licSlotData}ear
    } else /if (${Select[${ref},wrist,wrists,bracers,bracer]}) {
      /varset ${destVar} ${licSlotData}wrist
    } else /if (${Select[${ref},ring,rings,fingers,finger]}) {
      /varset ${destVar} ${licSlotData}finger
    } else /if (${Select[${ref},primary,secondary,weapon]}) {
      /if (${licSlotData.Equal[left]}) {
        /varset ${destVar} mainhand
      } else /if (${licSlotData.Equal[right]}) {
        /varset ${destVar} offhand
      }
    }
  } else {
    /if (${ref.Equal[primary]} || ${ref.Equal[main]}) {
      /varset ${destVar} mainhand
    } else /if (${ref.Equal[secondary]} || ${ref.Equal[off]}) {
      /varset ${destVar} offhand
    } else /if (${ref.Equal[range]}) {
      /varset ${destVar} ranged
    } else /if (${ref.Equal[belt]}) {
      /varset ${destVar} waist
    } else /if (${ref.Equal[shoulders]}) {
      /varset ${destVar} shoulder
    }
  }
  
/return


















