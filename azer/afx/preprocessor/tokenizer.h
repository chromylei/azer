#pragma once

#include <string>
#include "azer/afx/compiler/loc.h"
#include "base/basictypes.h"
#include "base/strings/string_tokenizer.h"

//
union YYSTYPE;

namespace azer {
namespace afx {
/**
 * class CodeParty
 * party the code into three part(code, comments and preprocessor)
 */
class CodeParty {
 private:
  enum State {
    kNotSpecified = 0,
    kInit,
    kCommentStartBegin,
    kLineComment,
    kComment,
    kCommentStartEnd,
    kCode,
    kPreprocessor,
    kEnd,
  };
 public:
  enum Part {
    kUnmatchedPart,
    kCodePart,
    kCommentPart,
    kPreprocessorPart,
    kEndPart,
  };
  CodeParty(const std::string& str)
      : code_(str)
      , current_(code_.c_str())
      , prev_current_(code_.c_str())
      , lineno_(0)
      , prev_lineno_(0)
      , status_(kInit)
      , prev_status_(kNotSpecified){
  }

  Part GetNextToken(std::string* content);
  int lineno() const { return prev_lineno_;}
  const char* current() const { return prev_current_;}
 private:
  State status() const { return status_;}
  void EnterStatus(State status);
  void RevertStatus();
  bool PrevStatusNeedReturn();
  Part GetLexForState(State status);
  Part RetAndSwitchStatus();

  const std::string code_;
  std::string rest_;
  std::string uncertain_;
  const char* current_;
  const char* prev_current_;
  int lineno_;
  int prev_lineno_;
  State status_;
  State prev_status_;
  DISALLOW_COPY_AND_ASSIGN(CodeParty);
};

/**
 * class PreprocessorTokenizer
 *
 */
class PreprocessorTokenizer {
 public:
  class Delegate {
   public:
    virtual void ReportError(const std::string& str, int lineno) = 0;
  };

  explicit PreprocessorTokenizer(const std::string& code)
      : code_party_(code)
      , prev_token_lex_(-1)
      , next_token_endline_(false)
      , delegate_(NULL) {
  }

  PreprocessorTokenizer(const std::string& code, Delegate* delegate)
      : code_party_(code)
      , prev_token_lex_(-1)
      , next_token_endline_(false)
      , delegate_(delegate) {
  }


  int GetNextToken(YYSTYPE* yylval);
  void ReadNextTokenUntilLineend() { next_token_endline_ = true;}
  int lineno() const { return code_party_.lineno();}
 private:
  int GetToken(YYSTYPE* yylval);
  char TokenizerPeerNext();
  int Tokenizer(const std::string& token, YYSTYPE* yylval, bool try_next);
  int GetCodeParty(std::string* content);
  int GetTokenUntilLineEnd(YYSTYPE* yylval);

  void ReportError(const std::string& str);
  CodeParty code_party_;
  std::string peek_token_;
  std::string prev_token_;
  int prev_token_lex_;
  
  std::string propressor_;
  std::unique_ptr< std::string> token_str_;
  std::unique_ptr< ::base::StringTokenizer> tokenizer_;
  bool next_token_endline_;
  Delegate* delegate_;
  DISALLOW_COPY_AND_ASSIGN(PreprocessorTokenizer);
};
}  // namespace afx
}  // namespace azer
