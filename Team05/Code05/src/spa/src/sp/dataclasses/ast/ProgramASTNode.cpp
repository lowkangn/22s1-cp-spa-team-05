#include <sp/dataclasses/ast/ProgramASTNode.h>

Entity ProgramASTNode::extractEntity() {
	Token undefinedToken = Token::getPlaceHolderToken();
	return Entity{ EntityType::PROGRAM, -1, undefinedToken };
}
