MenuAction_Cancel = 8
MenuAction_Display = 2
MenuAction_End = 16
MenuAction_Select = 4
MenuAction_Start = 1
SourceMod::MenuCancel_Disconnected = -1
SourceMod::MenuCancel_ExitBack = -6
SourceMod::MenuCancel_Interrupted = -2
SourceMod::MenuCancel_NoDisplay = -4
SourceMod::MenuCancel_Timeout = -5
SourceMod::MenuEnd_Cancelled = -3
SourceMod::MenuEnd_Exit = -4
SourceMod::MenuEnd_ExitBack = -5
SourceMod::MenuEnd_Selected = 0
SourceMod::MenuEnd_VotingCancelled = -2
SourceMod::MenuEnd_VotingDone = -1

class Menu:
  '''A Valve menu structure'''
  
  def display(self, client, time):
    '''display(client, time) -> bool
    
    Displays the menu to a client for the requested amount of time.
    @type  client: clients.Client or int
    @param client: A Client object or client index representing the client to
        display the menu to.
    @type  time: int
    @param time: The number of seconds to display the menu for.
    @rtype: bool
    @return: True if sucessfully displayed, False otherwise'''
    pass
  

