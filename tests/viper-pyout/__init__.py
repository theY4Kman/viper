import os, os.path

import inspect
import types

import sourcemod
from sourcemod import Plugin_Handled

myinfo = {
  'name': "Viper Headers Output",
  'author': "theY4Kman",
  'description': "Outputs barebones headers.",
  'version': "1.0"
}

def outmod(cmd):
  out = gen_mod(__import__(cmd.args[0]))
  
  fp = open(cmd.args[0] + '.py', 'w')
  fp.write(out[cmd.args[0]][0])
  fp.close()
  
  for line in out.splitlines():
    console.print_to_server(line)
  
  return Plugin_Handled

def output_header(base, mod):
  name = mod[0]
  out = mod[1][0]
  children = mod[1][1]
  
  if len(children) > 0:
    base += name + '/'
    
    if not os.path.exists(base):
      os.makedirs(base)
  
    fp = open(base + '__init__.py', 'w')
    fp.write(out)
    fp.close()
  
  for name,mod in children:
    out = mod[0]
    children = mod[1]
    
    if len(children) > 0:
      output_header(base + name + '/', (name, mod))
      continue
    
    fp = open(base + name + '.py', 'w')
    fp.write(out)
    fp.close()

def outmods(cmd):
  mod = gen_mod(sourcemod, deep=True)
  base = sourcemod.get_sourcemod_path() + '/extensions/viper/headers/'
  
  output_header(base, mod)
  
  cmd.reply('Successfully outputted headers')
  return Plugin_Handled

# Retrieve the function prototype from its docstring, as long as the first line
# is of the format: function_name(pro, to[, ty=pe])
def proto_from_doc(doc):
  begin = doc.find('(')
  end = doc.find(')')
  
  if begin == -1 or end == -1:
    return ''
  
  doc = doc[begin+1:end].replace(']', '').replace('[', '')
  doc = doc.replace('...', '*args')
  doc = doc.strip()
  
  return doc

# Indent all but the first line
def indent_doc(doc):
  lines = doc.splitlines()
  if len(lines) == 1:
    return doc
  
  return lines[0] + '\n' + '\n'.join(['  '+line for line in lines[1:]])

def isclassattr(o):
  return inspect.isgetsetdescriptor(o) or inspect.ismemberdescriptor(o)

def gen_mod(mod, indent=0, cls=False, deep=False):
  children = []
  out = ''
  used = []
  
  for pred in [inspect.ismodule, isclassattr, inspect.isclass,
               inspect.isroutine, inspect.isbuiltin, None]:
    # We want to output constants at the top
    if pred is None:
      _out = out
      out = ''
      
    for name,val in inspect.getmembers(mod, pred):
      if name.startswith('__') or name in used:
        # Discard these; they're useless in docs
        continue
      
      used.append(name)
      
      if inspect.ismodule(val) and deep is True:
        children.append(gen_mod(val))
        pass
      
      # A function in the toplevel of the module
      elif inspect.isbuiltin(val):
        out += "def %s(%s):\n  '''%s'''\n  pass\n\n" % (name,
            proto_from_doc(val.__doc__), indent_doc(val.__doc__))
      
      # A class
      elif inspect.isclass(val) and name != '__class__':
        out += "class %s:\n  '''%s'''\n%s\n" % (name, val.__doc__,
            gen_mod(val, indent+1, True)[1][0])
      
      # A method in a class
      elif inspect.isroutine(val):
        proto = proto_from_doc(val.__doc__)
        if proto != "":
          proto = 'self, ' + proto
        
        out += "def %s(%s):\n  '''%s'''\n  pass\n\n" % (name, proto,
            indent_doc(val.__doc__))
      
      # Regular ol' variables: in classes or constants in toplevel
      else:
        if inspect.isgetsetdescriptor(val) or inspect.ismemberdescriptor(val):
          doc = inspect.getdoc(val)
        else:
          doc = inspect.getcomments(val)
        
        s = 'None' if cls else repr(val)
        if s.startswith('<'):
          s = 'None'
        
        if doc:
          out += "'''%s'''\n%s = %s\n" % (doc, name, s)
        else:
          out += '%s = %s\n' % (name, s)
    
    if pred is None:
      out = out + '\n' + _out
  
  out = '\n'.join([('  '*indent)+line for line in out.splitlines()]) + '\n'
  return (mod.__name__, (out, children))

if __name__ == "__main__":
  sourcemod.console.reg_concmd("sm_dumpmod", outmod)
  sourcemod.console.reg_concmd("sm_dumpmods", outmods)