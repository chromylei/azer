#include "azer/afx/preprocessor/tokenizer.h"

#include "base/logging.h"
#include "base/strings/string_util.h"
#include "preprocessor_tab.h"


namespace azer {
namespace afx {
CodeParty::Part CodeParty::GetNextToken(std::string* content) {
  prev_lineno_ = lineno_;
  content->clear();
  content->append(rest_);
  rest_.clear();
  char prev = '\0';
  char c = '\0';
  const char* begin = code_.c_str();
  prev_current_ = current_;
  for (; current_ < begin + code_.length(); ++current_) {
    prev = c;
    c = *current_;
    switch (status()) {
      case kInit:
        if (c == '/') {
          content->push_back(c);
          EnterStatus(kCommentStartBegin);
        } else if (c == '#') {
          content->push_back(c);
          EnterStatus(kPreprocessor);
        } else {
          content->push_back(c);
          EnterStatus(kCode);
        }
        break;
      case kCommentStartBegin:
        if (c == '/') { 
          if (PrevStatusNeedReturn()) {
            EnterStatus(kLineComment);
            rest_.push_back(c);
            ++current_;
            return RetAndSwitchStatus();
          } else {
            EnterStatus(kLineComment);
            content->push_back(c);
          }
        } else if ( c == '*') {
          if (PrevStatusNeedReturn()) {
            EnterStatus(kComment);
            rest_.push_back(c);
            ++current_;
            return RetAndSwitchStatus();
          } else {
            EnterStatus(kComment);
            content->push_back(c);
          }
        } else {
          if (c == '\n') lineno_++;
          content->append(rest_);
          rest_.clear();
          content->push_back(c);
          EnterStatus(prev_status_);
        }
        break;
      case kLineComment:
        content->push_back(c);
        if (c == '\n') {
          lineno_++;
          ++current_;
          EnterStatus(kInit);
          return kCommentPart;
        } else {
          break;
        }
      case kComment:
        content->push_back(c);
        if (c == '*') {
          EnterStatus(kCommentStartEnd);
        } else if (c == '\n') lineno_++;
        break;
      case kCommentStartEnd:
        content->push_back(c);
        if (c == '/') {
          EnterStatus(kInit);
          ++current_;
          return kCommentPart;
        } if (c == '*') {
          break;
        } else {
          status_ = kComment;
          break;
        }
      case kCode:
        /*
        if (prev_status_ == kCommentStartBegin) {
          prevline_.push_back('/');
        }
        */

        if (c == '#') {
          rest_.push_back('#');
          ++current_;
          EnterStatus(kPreprocessor);
          return kCodePart;
        } else if (c == '/') {
          rest_.push_back(c);
          EnterStatus(kCommentStartBegin);
        } else if (c == '\n') {
          lineno_++;
          content->push_back('\n');
        } else {
          content->push_back(c);
        }
        break;
      case kPreprocessor:
        if (c == '\n') {
          content->push_back(c);
          lineno_++;
          if (prev != '\\') {
            ++current_;
            EnterStatus(kInit);
            return kPreprocessorPart;
          }
        } else if (c == '\r') {
        } else if (c == '/') {
          content->push_back(c);
          rest_.push_back(c);
          EnterStatus(kCommentStartBegin);
        } else {
          content->push_back(c);
        }
        break;
    }
  }

  if (content->size() == 0u) {
    // if return with \n, and then end with '/0'
    EnterStatus(kEnd);
    return kEndPart;
  } else if (status_ == kEnd) {
    return kEndPart;
  } else {
    EnterStatus(kEnd);
    return GetLexForState(prev_status_);
  }
}

void CodeParty::EnterStatus(State status) {
  if (status_ != kCommentStartBegin) {
    prev_status_ = status_;
  }
  status_ = status;
}

void CodeParty::RevertStatus() {
  DCHECK_NE(prev_status_, kNotSpecified);
  status_ = prev_status_;
  prev_status_ = kNotSpecified;
}

CodeParty::Part CodeParty::GetLexForState(State status) {
  switch (status) {
    case kLineComment:
    case kComment: return kCommentPart;
    case kPreprocessor: return kPreprocessorPart;
    case kEnd: return kEndPart;
    case kCode: return kCodePart;
    case kNotSpecified:
    case kInit:
    case kCommentStartBegin:
    case kCommentStartEnd:
    default:
      return kUnmatchedPart;
  }
}

CodeParty::Part CodeParty::RetAndSwitchStatus() {
  return GetLexForState(prev_status_);
}

bool CodeParty::PrevStatusNeedReturn() {
  return prev_status_ != kNotSpecified
      && prev_status_ != kInit;
}

// class PreprocessorTokenizer
int PreprocessorTokenizer::GetCodeParty(std::string* content) {
  CodeParty::Part part = code_party_.GetNextToken(content);
  if (part == CodeParty::kCodePart) {
    return PRE_CODE;
  } else if (part == CodeParty::kCommentPart) {
    return PRE_COMMENT;
  } else if (part == CodeParty::kEndPart) {
    return PRE_EOF;
  } else if (part == CodeParty::kPreprocessorPart) {
    return PRE_PROCESSOR;
  } else if (part == CodeParty::kUnmatchedPart) {
    return PRE_NOMATCH;
  } else {
    NOTREACHED();
    return PRE_NOMATCH;
  }
}

int PreprocessorTokenizer::Tokenizer(const std::string& token, YYSTYPE* yylval,
                                     bool try_next) {
  int tok = -1;
  yylval->lex.loc.lineno = lineno();
  prev_token_ = token;
  if (token == "#") {tok = PRE_SHARP;
  } else if (token == "(") {tok = PRE_LEFT_PAREN;
  } else if (token == ")") {tok = PRE_RIGHT_PAREN;
  } else if (token == "<") {
    if (try_next && tokenizer_->GetNext()) {
      std::string str = tokenizer_->token();
      if (str == "=") {
        tok = PRE_OP_LE;
      } else {
        tok = PRE_LEFT_ANGLE;
        peek_token_ = str;
      }
    } else {
      tok = PRE_LEFT_ANGLE;
    }
  } else if (token == ">") {
    if (try_next && tokenizer_->GetNext()) {
      std::string str = tokenizer_->token();
      if (str == "=") {
        tok = PRE_OP_GE;
        prev_token_.push_back('=');
      } else {
        peek_token_ = str;
        tok = PRE_RIGHT_ANGLE;
      }
    } else {
      tok = PRE_RIGHT_ANGLE;
    }
  } else if (token == "!") {
    if (try_next && tokenizer_->GetNext()) {
      std::string str = tokenizer_->token();
      if (str == "=") {
        tok = PRE_OP_NE;
        prev_token_.push_back('=');
      } else {
        peek_token_ = str;
        tok = PRE_BANG;
      }
    } else {
      tok = PRE_BANG;
    }
  } else if (token == "=") {
    if (try_next && tokenizer_->GetNext()) {
      std::string str = tokenizer_->token();
      if (str == "=") {
        prev_token_.push_back('=');
        tok = PRE_OP_EQ;
      } else {
        peek_token_ = str;
        tok = PRE_ASSIGN;
      }
    } else {
      tok = PRE_ASSIGN;
    }
  } else if (token == " " || token == "\t" || token == "\\" || token == "\n") {
    tok = GetToken(yylval);
  } else if (token == "define") {tok = PRE_DEFINE;
  } else if (token == "defined") {tok = PRE_DEFINED;
  } else if (token == "elif") {tok = PRE_ELIF;
  } else if (token == "else") {tok = PRE_ELSE;
  } else if (token == "endif") {tok = PRE_ENDIF;
  } else if (token == "include") {tok = PRE_INCLUDE;
  } else if (token == "ifndef") { tok = PRE_IFNDEF;
  } else if (token == "ifdef") { tok = PRE_IFDEF;
  } else if (token == "if") {tok = PRE_IF;
  } else if (token == "package") {tok = PRE_PACKAGE;
  } else if (token == "undef") {tok = PRE_UNDEF;
  } else if (StartsWithASCII(token, "\"", false) || EndsWith(token, "\"", false)) {
    yylval->lex.string = new std::string(token);
    tok = PRE_STRING;
  } else {
    yylval->lex.string = new std::string(token);
    tok = PRE_STRING;
  }

  return tok;
}

int PreprocessorTokenizer::GetToken(YYSTYPE* yylval) {
  yylval->lex.loc.lineno = lineno();
  if (next_token_endline_) {
    return GetTokenUntilLineEnd(yylval);
  } else if (!peek_token_.empty()) {
    int lex = Tokenizer(peek_token_, yylval, false);
    peek_token_.clear();
    return lex;
  } else if (tokenizer_.get() && tokenizer_->GetNext()) {
    return Tokenizer(tokenizer_->token(), yylval, true);
  } else {
    std::string str;
    int ret = GetCodeParty(&str);
    if (ret == PRE_PROCESSOR) {
      token_str_.reset(new std::string(str));
      tokenizer_.reset(new ::base::StringTokenizer(*token_str_.get(),
                                                   " \t()<>#!+-*/=\\\n"));
      tokenizer_->set_quote_chars("<\"");
      tokenizer_->set_options(::base::StringTokenizer::RETURN_DELIMS);
      return GetToken(yylval);
    } else if (ret == PRE_NOMATCH) {
      return PRE_NOMATCH;
    } else {
      yylval->lex.string = new std::string(str);
      return ret;
    }
  }
}

int PreprocessorTokenizer::GetTokenUntilLineEnd(YYSTYPE* yylval) {
  yylval->lex.string = new std::string;
  std::string token = peek_token_;
  yylval->lex.string->append(token);
  while (tokenizer_.get() && tokenizer_->GetNext()) {
    token = tokenizer_->token();
    yylval->lex.string->append(token);
    if (token != "\t" || token != " ") {
      prev_token_ = token;
    }
    if (token == "\n" && prev_token_ != "\\") {
      break;
    }
  }

  prev_token_ = *yylval->lex.string;
  next_token_endline_ = false;
  return PRE_STRING;
}

void PreprocessorTokenizer::ReportError(const std::string& str) {
  if (delegate_) {
    delegate_->ReportError(str, lineno());
  }
}
const char* token_name(int type) {
  switch (type) {
    case 0: return "YYEOF";
    case PRE_CODE: return "PRE_CODE";
    case PRE_COMMENT: return "PRE_COMMENT";
    case PRE_PROCESSOR: return "PRE_PROCESSOR";
    case PRE_EOF: return "PRE_EOF";
    case PRE_NOMATCH: return "PRE_NOMATCH";
    case PRE_BANG: return "PRE_BANG";
    case PRE_SHARP: return "PRE_SHARP";
    case PRE_LEFT_PAREN: return "PRE_LEFT_PAREN";
    case PRE_RIGHT_PAREN: return "PRE_RIGHT_PAREN";
    case PRE_ASSIGN: return "PRE_ASSIGN";
    case PRE_OP_GE: return "PRE_OP_GE";
    case PRE_OP_LE: return "PRE_OP_LE";
    case PRE_OP_EQ: return "PRE_OP_EQ";
    case PRE_OP_NE: return "PRE_OP_NE";
    case PRE_LEFT_ANGLE: return "PRE_LEFT_ANGLE";
    case PRE_RIGHT_ANGLE: return "PRE_RIGHT_ANGLE";
    case PRE_STRING: return "PRE_STRING";
    case PRE_DEFINED: return "PRE_DEFINED";
    case PRE_DEFINE: return "PRE_DEFINE";
    case PRE_INCLUDE: return "PRE_INCLUDE";
    case PRE_UNDEF: return "PRE_UNDEF";
    case PRE_IF: return "PRE_IF";
    case PRE_ENDIF: return "PRE_ENDIF";
    case PRE_ELSE: return "PRE_ELSE";
    case PRE_ELIF: return "PRE_ELIF";
    case PRE_IFDEF: return "PRE_IFDEF";
    case PRE_IFNDEF: return "PRE_IFNDEF";
      case PRE_PACKAGE: return "PRE_PACKAGE";
    default: NOTREACHED() << type; return "";
  }
}

int PreprocessorTokenizer::GetNextToken(YYSTYPE* yylval) {
  if (prev_token_lex_ == PRE_EOF) {
    return 0;
  } else {
    prev_token_lex_ = GetToken(yylval);
    // LOG(ERROR) << token_name(prev_token_lex_);
    return prev_token_lex_;
  }
}
}  // namespace afx
}  // namespace azer
