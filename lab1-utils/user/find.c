#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  char *p_buf = buf;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  // no blank padding
  while (*p && (*p != ' ')) {
    *p_buf++ = *p++;
  }
  *p_buf = '\0';

  return buf;
}

// output -> path/filename
void relative_path_name(char *path, char *filename, char *dst) {
  char *p = dst;
  while (*path && (*path != ' ')) {
    *p++ = *path++;
  }
  *p++ = '/';
  while (*filename && (*path != ' ')) {
    *p++ = *filename++;
  }
  *p = '\0';
}

int is_name_correct(char *name2check, char *name) {
    if (name == 0) {
        return 1;
    }
    return !strcmp(name2check, name);
}

void find(char *path, char *name)
{
  char new_path[512];
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if (is_name_correct(fmtname(path), name))
        printf("%s\n", path);
    break;

  case T_DIR:
    if (is_name_correct(path, name))
      printf("%s\n", path);

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if (de.inum == 0)
        continue;
      if (de.name[0] == '.')  // ingore . ..
        continue;

      relative_path_name(path, de.name, new_path);
      find(new_path, name);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  if(argc < 2) {
    find(".", 0);
    exit(0);
  }
  else if (argc == 2) {
    find(argv[1], 0);
  } else {
    find(argv[1], argv[2]);
  }
  exit(0);
}