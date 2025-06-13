/*
 * Copyright Raffaele Rossi 2025.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */
#include "slick.sh"
#include "tagsdb.sh"

static int _run(...) {
  _str args[];
  for (i=0; i < arg() ; ++i)
    args[i] = arg(i+1);
  cmd = join(args, " ");
  clear_output_window();
  activate_output();
  rv := exec_command_to_window(cmd, output_window_text_control());
  activate_editor();
  return rv;
}

static int _dep0(...) {
  _str cmd[];
  for (i=0; i < arg() ; ++i)
    cmd[i] = arg(i+1);

  return _run("dep0", p_buf_name, join(cmd, " "));
}

_command Dep0TypeCheck() {
  if (save())
    message("Error saving file");
  else if (_dep0("-t"))
    message("Erroring running dep0 command");
  else
    message("This file type-checks correctly");
}

_command Dep0GenLLVM() {
  if (save())
    message("Error saving file");
  else if (_dep0("--skip-transformations", "--emit-llvm", "-o", "/dev/stdout"))
    message("Erroring running dep0 command");
  else
    message("Done.");
}

_command Dep0GenLLVMUnverified() {
  if (save())
    message("Error saving file");
  else if (_dep0("--skip-transformations", "--emit-llvm-unverified", "-o", "/dev/stdout"))
    message("Erroring running dep0 command");
  else
    message("Done.");
}
