#Event CheckExpGroup "[MQ2] CheckExpGroup #1#"
Sub Event_CheckExpGroup(line, input)
  /declare CHECK_ExpGroup int outer 1
  /call Event_LoadExpGroup "${line}" "${input}"
  /deletevar CHECK_ExpGroup
/return

#Event LoadExpGroup "[MQ2] LoadExpGroup #1#"
Sub Event_LoadExpGroup(line, input)

  /if (${Defined[CurrentExpGroup]} ) /call DisbandExpGroup "${CurrentExpGroup}"

  /call #CreateVars string outer CharList Term1 Term2 Term3 Term4 Term5 Term6 Slot1 Slot2 Slot3 Slot4 Slot5 Slot6 FoundChar ExpGroup_Ini TermReturn Template CurrentExpGroup
  /call #CreateVars int outer NextSlot RemainingSlots RemainingInfoReports ExecutingLoadExpGroup SlotsLastIteration BestCharIndex RecursionDepth NameLength NamePosition
  
  /varset ExpGroup_Ini "e3 Macro Inis\\e3_ExpGroup.ini"
  /call GetExpTemplate "${input}" Template
  /echo Template: ${Template}

  /varset NextSlot 1
  /varset RemainingSlots 6
  
  /declare i int local
  /declare termCount int local ${Math.Calc[1+${Template.Count[|]}]}
  
  /call GetNetbotsCharactersInZone CharList
  /echo CharList: ${CharList}
  
  /call #CreateArrays string outer ExpCharInfo[${Math.Calc[${CharList.Count[|]} - 1]}]

  /call GetExpCharInfo
  /call SortExpCharInfo
  /call #PrintArray ExpCharInfo
  /call ResetBestIndex
  
  /if (${Defined[Debug_Exp]} && ${Debug_Exp}) /call PrintArray ExpCharInfo

  /for i 1 to ${termCount}
    /call ClassifyTemplateTerm "${Template.Arg[${i},|]}" Term${i}
  /next i
  
  |/if (${Defined[Debug_Exp]} && ${Debug_Exp}) /call PrintVars Term1 Term2 Term3 Term4 Term5 Term6
  |/if (${Defined[Debug_Exp]} && ${Debug_Exp}) /call PrintVars Slot1 Slot2 Slot3 Slot4 Slot5 Slot6

  | Fill up any top-level char-specific slots first
  /for i 1 to ${termCount}
    /if (${Term${i}.Arg[1,:].Equal[CHAR]} && ${CharList.Find[|${Term${i}.Arg[2,:]}|]}) {
      |/echo ${Term${i}} = ${Term${i}.Arg[2,:]}
      /call AssignCharToSlot ${Term${i}.Arg[2,:]}
      /varset Term${i}
    }
  /next i
  
  | Then fill up any top-level class-specific slots
  /for i 1 to ${termCount}
    /if (${Term${i}.Arg[1,:].Equal[CLASS]}) {
      /call FindCharOfClass ${Term${i}.Arg[2,:]}
      /if (${BestCharIndex}<=${ExpCharInfo.Size}) {
        /varset FoundChar ${ExpCharInfo[${BestCharIndex}].Arg[1,|]}
        /call ResetBestIndex
      }
      /if (${FoundChar.Length}) {
        /call AssignFoundChar Term${i}
      }
    }
  /next i
  
  :Loop
    /for i 1 to ${termCount}
      /if (${Term${i}.Length}) /call FindCharForTerm "${Term${i}}"
      /if (${FoundChar.Length}) {
        /call AssignFoundChar Term${i}
      }
    /next i
    /call CheckCompletionStatus
  /if (!${Macro.Return}) /goto :Loop
  /if (${Defined[Debug_Exp]} && ${Debug_Exp}) /call PrintSlots
  /varset CurrentExpGroup ${Slot1} ${Slot2} ${Slot3} ${Slot4} ${Slot5} ${Slot6}
  /echo New Group: ${CurrentExpGroup}
  /if (${Defined[CHECK_ExpGroup]}) /return
  /call CreateExpGroup "${CurrentExpGroup}"
  
  /if (${Defined[Use_FakeLesson]} && ${Use_FakeLesson}) {
    /bc FakeLesson 0 /only|${CurrentExpGroup}
  }
  |/mode NoKS off
  |/mode NoKS on /not|${CurrentExpGroup}
  /expgroup off
  /expgroup on /only|${CurrentExpGroup}
  /deletevar ExecutingLoadExpGroup
/return

Sub GetNetbotsCharactersInZone(destVar)
  /declare i int local
  /varset ${destVar} |
  /for i 1 to ${Math.Calc[1+${NetBots.Client.Count[ ]}]}
    |/if (${NetBots[${NetBots.Client.Arg[${i}, ]}].InZone}) {
    /if (${NetBots[${NetBots.Client.Arg[${i}, ]}].InZone} && ${NetBots.Client.Arg[${i}, ].NotEqual[GMFizban]} ) {
      /varset ${destVar} ${${destVar}}${NetBots.Client.Arg[${i}, ]}|
      |/echo destVar ${${destVar}}
    }
  /next i
/return

Sub ClassifyTemplateTerm(term, destVar)
  /if (${Select[${term},MAG,WIZ,ENC,NEC,DRU,BST,MNK,ROG,SHM,BER,RNG,WAR,PAL,SHD,CLR,BRD]}) {
    /varset ${destVar} CLASS:${term}
    /return
  }
  /if (${Ini[${ExpGroup_Ini},Types,${term}].Length}) {
    /varset ${destVar} TYPE:${Ini[${ExpGroup_Ini},Types,${term}]}
    /return
  }
  /if (${NetBots.Client.Find[ ${term}]} || ${NetBots.Client.Left[${term.Length}].Equal[${term}]}) {
    /varset ${destVar} CHAR:${term}
    /return
  }
  /if (${term.Lower.Equal[none]}) {
    /varset ${destVar} NONE:NONE
    /return
  }
  /varset ${destVar} UNKNOWN:${term}
/return

Sub FindCharForTerm(Term)
  /if (${Term.Arg[1,:].Equal[NONE]}) /return
  /varcalc RecursionDepth ${RecursionDepth}+1
  /if (${Term.Arg[1,:].Equal[CHAR]}) {
    /if (${CharList.Find[|${Term.Arg[2,:]}|]}) {
      /call GetCharInfoIndex ${Term.Arg[2,:]}
      /if (${Macro.Return}<${BestCharIndex}) /varset BestCharIndex ${Macro.Return}
    }
  } else /if (${Term.Arg[1,:].Equal[CLASS]}) {
    /call FindCharOfClass ${Term.Arg[2,:]}
  } else /if (${Term.Arg[1,:].Equal[TYPE]}) {
    /declare i int local
    /for i 1 to ${Math.Calc[1+${Term.Arg[2,:].Count[|]}]}
      /call ClassifyTemplateTerm "${Term.Arg[2,:].Arg[${i},|]}" TermReturn
      /call FindCharForTerm "${TermReturn}"
    /next i
  } else {
    /echo Term Error: ${Term}
  }
  /varcalc RecursionDepth ${RecursionDepth}-1
  /if (${RecursionDepth}==0) {
    /if (${BestCharIndex}<=${ExpCharInfo.Size}) {
      /varset FoundChar ${ExpCharInfo[${BestCharIndex}].Arg[1,|]}
      /call ResetBestIndex
    }
  }
/return

Sub FindCharOfClass(cls)
  /declare i int local
  /for i 1 to ${ExpCharInfo.Size}
    /if (${CharList.Find[|${ExpCharInfo[${i}].Arg[1,|]}|]} && ${NetBots[${ExpCharInfo[${i}].Arg[1,|]}].Class.ShortName.Equal[${cls}]} && ${i}<${BestCharIndex}) {
      /varset BestCharIndex ${i}
      /return
    }
  /next i
/return

Sub GetCharInfoIndex(Name)
  /declare i int local
  /for i 1 to ${ExpCharInfo.Size}
    /if (${ExpCharInfo[${i}].Arg[1,|].Equal[${Name}]}) /return ${i}
  /next i
/return ${Math.Calc[1+${ExpCharInfo.Size}].Int}

Sub AssignCharToSlot(Name)
  /if (!${RemainingSlots}) /return
  /echo Assign ${Name} to Slot${NextSlot}
  /varset Slot${NextSlot} ${Name}
  /varcalc NextSlot ${NextSlot}+1
  /varcalc RemainingSlots ${RemainingSlots}-1

  /varset NamePosition ${CharList.Find[|${Name}|]}
  /varset NameLength ${Math.Calc[${Name.Length} + 1]}
  /varset CharList ${CharList.Left[${NamePosition}]}${CharList.Right[-${Math.Calc[${NamePosition}+${NameLength}]}]}
/return

Sub AssignFoundChar(SourceTerm)
  
  /call AssignCharToSlot ${FoundChar}
  /varset ${SourceTerm}
  /varset FoundChar
  
/return

Sub GetExpCharInfo
  /doevents flush ExpInfoReturn
  /varset RemainingInfoReports ${Math.Calc[${CharList.Count[|]}-1]}

  /declare FinishExpReportTimer timer local 15s
  /if (${Defined[Use_FakeLesson]} && ${Use_FakeLesson}) {
    /noparse /bcaa //bc EXP:${Me.AAPointsTotal}-${If[${Defined[FakeLesson]},${FakeLesson},1]}-${Me.Level}
  } else {
    /noparse /bcaa //bc EXP:${Me.AAPointsTotal}-${If[${Me.AltAbilityReady[Lesson of the Devoted]},1,0]}-${Me.Level}
  }
  :Loop
    /doevents ExpInfoReturn
    /delay 1
  /if (${RemainingInfoReports} && ${FinishExpReportTimer}) /goto :Loop
/return

#Event SetFakeLesson "<#1#> FakeLesson #2#"
Sub Event_SetFakeLesson(line, chatSender, args)
  /call #CreateVars string outer ValuePart InclusionPart
  /call #SplitInclusionArgs "${args}" ValuePart InclusionPart
  
  /if (${InclusionPart.Length}) {
    /call #verifyEvent "${chatSender}" "${InclusionPart}"
    /if (!${Macro.Return}) /return
  }
  /call #CreateVars string outer FakeLesson
  
  /varset FakeLesson ${ValuePart}
  /bc FakeLesson: ${FakeLesson}
  
/return

#Event UseFakeLesson "<#1#> UseFakeLesson #2#"
Sub Event_UseFakeLesson(line, chatSender, newValue)
  /call #CreateVars int outer Use_FakeLesson
  /varset Use_FakeLesson ${newValue}
/return

#Event ExpInfoReturn "<#1#> EXP:#2#-#3#-#4#"
Sub Event_ExpInfoReturn(line, ChatSender, AAPoints, LessonAvailable, CharLevel)
|/echo RemainingInfoReports ${RemainingInfoReports}
  /if (!${Defined[ExecutingLoadExpGroup]}) /return
  /if (!${NetBots[${ChatSender}].InZone}) /return
  /if (${RemainingInfoReports} < 1) /return
  /varset ExpCharInfo[${RemainingInfoReports}] ${ChatSender}|${AAPoints}|${If[${LessonAvailable},0,1]}|${CharLevel}
  /varcalc RemainingInfoReports ${RemainingInfoReports}-1
/return

| Split ExpCharInfo into 2 arrays, with and without lesson, sort each and recombine.
Sub SortExpCharInfo
  /if (!${Defined[MaxLevel]}) /call #IniToVar "${genSettings_Ini},General,Max Level,70" MaxLevel int outer
|/echo ${MaxLevel}

  /declare i int local
  /declare counter int local
  /declare underLevelCount int local 0
  /for i 1 to ${ExpCharInfo.Size}
    /if (${ExpCharInfo[${i}].Arg[4,|]} < ${MaxLevel}) {
      /varcalc underLevelCount ${underLevelCount}+1
    } else {
      /if (${ExpCharInfo[${i}].Arg[3,|].Equal[0]}) /varcalc counter ${counter}+1
    }
  /next i
|/echo Count: ${ExpCharInfo.Size}
|/echo underlevel: ${underLevelCount}
|/echo Lesson Ready: ${counter}
|/echo Rest: ${Math.Calc[${ExpCharInfo.Size}-${counter}-${underLevelCount}]}

  /declare overLevelLessonNotReady int local ${Math.Calc[${ExpCharInfo.Size}-${counter}-${underLevelCount}]}
  /call #CreateArrays string outer CharInfoUnderLevelCap[${underLevelCount}] CharInfoWithLesson[${Math.Calc[${counter}]}] CharInfoNoLesson[${Math.Calc[${ExpCharInfo.Size}-${counter}-${underLevelCount}]}]
|/echo CharInfoUnderLevelCap: ${CharInfoUnderLevelCap.Size}

  /declare LessonIter int local 1
  /declare NoLessonIter int local 1
  /declare UnderIter int local 1
  /for i 1 to ${ExpCharInfo.Size}
|/echo ${ExpCharInfo[${i}].Arg[1,|]}: ${ExpCharInfo[${i}].Arg[4,|]}
    /if (${ExpCharInfo[${i}].Arg[4,|]} < ${MaxLevel}) {
|/echo Under Level
      /varset CharInfoUnderLevelCap[${UnderIter}] ${ExpCharInfo[${i}]}
      /varcalc UnderIter ${UnderIter}+1
    } else {
|/echo Over Level
      /if (${ExpCharInfo[${i}].Arg[3,|].Equal[0]}) {
        /varset CharInfoWithLesson[${LessonIter}] ${ExpCharInfo[${i}]}
        /varcalc LessonIter ${LessonIter}+1
      } else {
        /varset CharInfoNoLesson[${NoLessonIter}] ${ExpCharInfo[${i}]}
        /varcalc NoLessonIter ${NoLessonIter}+1
      }
    }
  /next i
  /call #QuickSort CharInfoWithLesson 1 ${CharInfoWithLesson.Size} .Arg[2,|]
  /call #QuickSort CharInfoNoLesson 1 ${CharInfoNoLesson.Size} .Arg[2,|]
  /call #QuickSort CharInfoUnderLevelCap 1 ${CharInfoUnderLevelCap.Size} .Arg[3,|]
  /call #CombineArrays ExpCharInfo CharInfoUnderLevelCap CharInfoWithLesson CharInfoNoLesson

/return

| If all slots are filled or there are no terms left (or no chars left!), we are done.
| Also end if the previous iteration resulted in no change.
Sub CheckCompletionStatus
  /declare i int local
  /declare counter int 0
  /declare complete int local 0
  /for i 1 to 6
    /if (${Slot${i}.Length}) /varcalc counter ${counter}+1
  /next i
  /if (${counter}==6) /return 1
  /if (${counter}==${SlotsLastIteration}) /return 1
  /varcalc SlotsLastIteration ${counter}
  /varcalc counter 0
  /for i 1 to 6
    /if (!${Term${i}.Length}) /varcalc counter ${counter}+1
  /next i
  /if (${counter}==6) /return 1
  /if (${CharList.Length} == 1) /return 1
/return 0

Sub ResetBestIndex
  /varcalc BestCharIndex ${ExpCharInfo.Size}+1
/return

Sub GetExpTemplate(input, destVar)
  /if (${Ini[${ExpGroup_Ini},Types,${input}].Length}) {
    /varset ${destVar} ${Ini[${ExpGroup_Ini},Types,${input}]}
  } else {
    /varset ${destVar} ${input}
  }
  |/echo ${destVar}: ${${destVar}}
/return 

Sub CreateExpGroup(newGroup)
  /declare i int local
  /declare currChar string local
  /declare groupLeader string local
  /varset groupLeader ${newGroup.Arg[1, ]}
  /for i 1 to ${Math.Calc[${newGroup.Count[ ]}+1]}
    /squelch /bct ${newGroup.Arg[${i}, ]} //keypress F1
    /squelch /bct ${newGroup.Arg[${i}, ]} //disband
  /next i

  /delay 20

  /for i 2 to ${Math.Calc[${newGroup.Count[ ]}+1]}
    /squelch /bct ${groupLeader} //invite ${newGroup.Arg[${i}, ]}
  /next i
  /delay 20
  /for i 2 to ${Math.Calc[${newGroup.Count[ ]}+1]}
    /squelch /bct ${newGroup.Arg[${i}, ]} //invite
  /next i
  |/bct ${groupLeader} //mode reportexp on /only|${groupLeader}
/return

Sub DisbandExpGroup(oldgroup)
  /declare i int local
  /declare currChar string local
  /declare groupLeader string local
  /varset groupLeader ${oldgroup.Arg[1, ]}
  /for i 1 to ${Math.Calc[${oldgroup.Count[ ]}+1]}
    /bct ${oldgroup.Arg[${i}, ]} //disband
  /next i

  |/bct ${groupLeader} //mode reportexp off /only|${groupLeader}
/return

Sub PrintTerms
  /declare i int local
  /for i 1 to 6
    /if (${Term${i}.Length}) /echo Term${i} = '${Term${i}}'
  /next i
/return

Sub PrintSlots
  /declare i int local
  /for i 1 to 6
    /if (${Slot${i}.Length}) /echo Slot${i} = '${Slot${i}}'
  /next i
/return