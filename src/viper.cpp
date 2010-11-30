/**
 * =============================================================================
 * Viper
 * Copyright (C) 2007-2010 Zach "theY4Kman" Kanzler
 * Copyright (C) 2004-2007 AlliedModders LLC.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "viper.h"
#include "python/init.h"
#include "viper_globals.h"
#include "PluginSys.h"

SH_DECL_HOOK1_void(IServerGameDLL, GameFrame, SH_NOATTRIB, false, bool);

SourceMod::IRootConsole *g_pMenu = NULL;
SourceHook::CallClass<IServerGameDLL> *g_pGameDLLPatch = NULL;

bool
BaseViper::OnViperLoad(char *error, size_t maxlength, bool late)
{
    g_pGameDLLPatch = SH_GET_CALLCLASS(gamedll);
    SM_GET_IFACE(ROOTCONSOLE, g_pMenu);
    
    m_EmptyTuple = PyTuple_New(0);
    
    StartViper();
    
    return true;
}

void
BaseViper::StartViper()
{
    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, GameFrame, gamedll, this, &BaseViper::GameFrame, false);
    
    /* Notify! */
    ViperGlobalClass *pBase = ViperGlobalClass::head;
    while (pBase)
    {
        pBase->OnViperStartup(false);
        pBase = pBase->m_pGlobalClassNext;
    }

    /* Notify! */
    pBase = ViperGlobalClass::head;
    while (pBase)
    {
        pBase->OnViperAllInitialized();
        pBase = pBase->m_pGlobalClassNext;
    }

    /* Notify! */
    pBase = ViperGlobalClass::head;
    while (pBase)
    {
        pBase->OnViperAllInitialized_Post();
        pBase = pBase->m_pGlobalClassNext;
    }
    
    /* Setup the game frame hook */
    m_GameFrame = g_Forwards.CreateForward("game_frame", ET_Ignore,
        m_EmptyTuple, NULL);
    
    /* Load the plugins */
    char plugins_path[PLATFORM_MAX_PATH];
    g_pSM->BuildPath(SourceMod::Path_SM, plugins_path, sizeof(plugins_path),
        "plugins");
    g_VPlugins.LoadPluginsFromDir(plugins_path);
}

void
BaseViper::OnViperUnload()
{
    ViperGlobalClass *pBase = ViperGlobalClass::head;
    while (pBase)
    {
        pBase->OnViperShutdown();
        pBase = pBase->m_pGlobalClassNext;
    }
    
    pBase = ViperGlobalClass::head;
    while (pBase)
    {
        pBase->OnViperAllShutdown();
        pBase = pBase->m_pGlobalClassNext;
    }
    
    Py_DECREF(m_EmptyTuple);
}

/* Oh, I just love copying code directly from SourceMod
 * It warms the cockles of my heart. My cockles are warmed.
 */
void
BaseViper::PushCommandStack(const CCommand *cmd)
{
    CachedCommandInfo info;

    info.args = cmd;
#if SOURCE_ENGINE < SE_ORANGEBOX
    strncopy(info.cmd, cmd->Arg(0), sizeof(info.cmd));
#endif

    m_CommandStack.push(info);
}

char const *
BaseViper::CurrentCommandName()
{
#if SOURCE_ENGINE >= SE_ORANGEBOX
    return m_CommandStack.front().args->Arg(0);
#else
    return m_CommandStack.front().cmd;
#endif
}

const CCommand *
BaseViper::PeekCommandStack()
{
    if (m_CommandStack.empty())
        return NULL;

    return m_CommandStack.front().args;
}

void
BaseViper::PopCommandStack()
{
    m_CommandStack.pop();
}

void
BaseViper::GameFrame(bool simulating)
{
    if (m_GameFrame->GetFunctionCount())
        m_GameFrame->Execute(NULL, m_EmptyTuple);
}

BaseViper g_Viper;

ViperGlobalClass *ViperGlobalClass::head = NULL;
ViperGlobalClass::ViperGlobalClass()
{
    m_pGlobalClassNext = ViperGlobalClass::head;
    ViperGlobalClass::head = this;
}

/* Utility functions */
size_t
UTIL_Format(char *buffer, size_t maxlength, char const *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    size_t len = vsnprintf(buffer, maxlength, fmt, ap);
    va_end(ap);

    if (len >= maxlength)
    {
        buffer[maxlength - 1] = '\0';
        return (maxlength - 1);
    }
    else
    {
        return len;
    }
}

char *
sm_strdup(char const *str)
{
    char *ptr = new char[strlen(str)+1];
    strcpy(ptr, str);
    return ptr;
}

int
StrReplace(char *str, char const *from, char const *to, int maxlen) 
{
    char *pstr = str;
    int fromlen = strlen(from);
    int tolen = strlen(to);
    int RC = 0;

    while (*pstr != '\0' && pstr - str < maxlen) {
        if (strncmp(pstr, from, fromlen) != 0) {
            *pstr++;
            continue;
        }
        memmove(pstr + tolen, pstr + fromlen, maxlen - ((pstr + tolen) - str) - 1);
        memcpy(pstr, to, tolen);
        pstr += tolen;
        RC++;
    }
    return RC;
}

char const *
GetLastFolderOfPath(char const *path)
{
    char const *lastslash = strrchr(path, '/')-1;
    
    /* No slashes found, so just send 'er on back */
    if (lastslash == NULL)
        return path;
    
    char *seclastslash = const_cast<char*>(lastslash);
    while (seclastslash > path)
    {
        if (*(--seclastslash) == '/')
            break;
    }
    
    size_t length = lastslash - seclastslash + 1;
    char *nicename = (char*)malloc(length);
    
    strncpy(nicename, seclastslash+1, length);
    nicename[lastslash-seclastslash] = '\0';

    return static_cast<char const *>(nicename);
}

char const *
GetLastOfPath(char const *path)
{
    char const *lastslash = strrchr(path, '/');
    if (lastslash == NULL)
        return path;
    
    return lastslash + 1;
}

unsigned int
strncopy(char *dest, char const *src, size_t count)
{
    if (!count)
    {
        return 0;
    }

    char *start = dest;
    while ((*src) && (--count))
    {
        *dest++ = *src++;
    }
    *dest = '\0';

    return (dest - start);
}

