#include "excl_ana.h"

int main(int argc, char *argv[]){

  if (argc!=3){
    std::cout<<"You need to supply a file or a regexp and a var_file"<<std::endl;
    return 1;
  }
  
  excl_ana t(argv[1],argv[2]);
  t.Loop();
  std::cout<<"end"<<std::endl;
  return 0;
}

