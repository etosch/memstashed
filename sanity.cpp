 #include "Stash.h"
// includes Bucket.h
#include "Parser.h"
// includes Cmd.h
#include <cassert>


int main(){
	assert(Bucket::test());
	assert(Stash::test());
	assert(Parser::test());
	return 0;
}
