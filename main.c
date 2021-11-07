#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

extern int errno;

typedef enum {
  CHO_G_IDX,
  CHO_GG_IDX,
  CHO_N_IDX,
  CHO_D_IDX,
  CHO_DD_IDX,
  CHO_R_IDX,
  CHO_M_IDX,
  CHO_B_IDX,
  CHO_BB_IDX,
  CHO_S_IDX,
  CHO_SS_IDX,
  CHO_O_IDX,
  CHO_J_IDX,
  CHO_JJ_IDX,
  CHO_K_IDX,
  CHO_T_IDX,
  CHO_P_IDX,
  CHO_H_IDX,
} ChoIdx;

typedef enum {
  JUNG_A_IDX,
  JUNG_AI_IDX,
  JUNG_YA_IDX,
  JUNG_YAI_IDX,
  JUNG_EO_IDX,
  JUNG_EOI_IDX,
  JUNG_YEO_IDX,
  JUNG_YEOI_IDX,
  JUNG_O_IDX,
  JUNG_OA_IDX,
  JUNG_OAI_IDX,
  JUNG_OI_IDX,
  JUNG_YO_IDX,
  JUNG_U_IDX,
  JUNG_UEO_IDX,
  JUNG_UEOI_IDX,
  JUNG_UI_IDX,
  JUNG_YU_IDX,
  JUNG_EU_IDX,
  JUNG_EUI_IDX,
  JUNG_I_IDX,
} JungIdx;

typedef enum {
  JONG_NONE_IDX,
  JONG_G_IDX,
  JONG_GG_IDX,
  JONG_GS_IDX,
  JONG_N_IDX,
  JONG_NJ_IDX,
  JONG_NH_IDX,
  JONG_D_IDX,
  JONG_R_IDX,
  JONG_RG_IDX,
  JONG_RM_IDX,
  JONG_RB_IDX,
  JONG_RS_IDX,
  JONG_RT_IDX,
  JONG_RP_IDX,
  JONG_RH_IDX,
  JONG_M_IDX,
  JONG_B_IDX,
  JONG_BS_IDX,
  JONG_S_IDX,
  JONG_SS_IDX,
  JONG_O_IDX,
  JONG_J_IDX,
  JONG_CH_IDX,
  JONG_K_IDX,
  JONG_T_IDX,
  JONG_P_IDX,
  JONG_H_IDX,
} JongIdx;

typedef enum {
  ANIMAL_VOICE,
  NORMAL_TRASH,
  EARPHONE_BREAKER,
  NOISE_BOY,
  ILLUMINATI,
  UUK,
  NALGANGDU,
  SUNDUBU_VILLAIN,
  JEONUN_SALRYEOJUSEYO,
  CHROME_OS,
  WINDOWS_WEB_BROWSER,
  ZEROTWO_CHUGO_SIPDA
} Cmd;

typedef struct {
  unsigned chunk_size;
  unsigned idx;
  unsigned size;
  wchar_t *arr;
} Field;

typedef struct {
  unsigned hangul_idx;
  ChoIdx cho_idx;
  JungIdx jung_idx;
  JungIdx extending_jung_idx;
  JongIdx jong_idx;
  wchar_t letter;
} Hangul;

typedef struct {
  wchar_t ptrn[9 + 1];
  unsigned lens[9 + 1];
  unsigned idx;
} Ptrn;

void fgoline(FILE *, const unsigned);
void init_field(Field *, const unsigned);
void free_field(Field *);
void inc_field_up_to(Field *, const unsigned);
void init_hangul_with_letter(Hangul *, const wchar_t);
void init_hangul_with_idxs(Hangul *, const ChoIdx, const JungIdx, const JongIdx);
void init_hangul_none(Hangul *);
void set_hangul_jong(Hangul *, const JongIdx);
void unset_hangul_jong(Hangul *);
void init_ptrn(Ptrn *);
void add_ptrn(Ptrn *, const wchar_t, const unsigned);
void end_ptrn(Ptrn *);
void adjust_ptrn(Ptrn *, const unsigned, const int);
bool match_ptrn(const Ptrn *, const wchar_t *);
bool is_ptrn_full(const Ptrn *);

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");

  FILE *src;
  wchar_t c;
  Field field;
  Cmd cmd;
  unsigned argument;
  Ptrn ptrn;
  Hangul hangul;
  Hangul cur_hangul;
  unsigned cur_len = 0;
  bool is_first_letter;
  wchar_t prev_c = 0;
  bool run = true;
  int exit_status = EXIT_SUCCESS;
  unsigned i;

  if (argc == 1) {
    fprintf(stderr, "그저 현상현\n");
    return 1;
  }

  src = fopen(argv[1], "r,ccs=UTF-8");
  if (!src) {
    fprintf(stderr, "아 또 현상현이네\n");
    fclose(src);
    return 2;
  }

  init_field(&field, 128);
  init_ptrn(&ptrn);
  cur_len = 0;
  init_hangul_none(&cur_hangul);
  is_first_letter = true;
  while (run && prev_c != WEOF) {
    c = fgetwc(src);

    if (c != L'\n' && c != L'\0') {
      if (is_ptrn_full(&ptrn)) {
        errno = EINVAL;
        run = false;
        continue;
      }

      if (c == ' ') {
        if (is_first_letter || prev_c == ' ') {
          fprintf(stderr, "상현아!!!");
          run = false;
        }

        if (cur_len > 0) {
          unset_hangul_jong(&cur_hangul);
          add_ptrn(&ptrn, cur_hangul.letter, cur_len);
        }

        add_ptrn(&ptrn, L' ', 1);

        init_hangul_none(&cur_hangul);

        cur_len = 0;

        continue;
      }

      init_hangul_with_letter(&hangul, c);

      if (hangul.cho_idx == CHO_O_IDX &&
          hangul.jung_idx == cur_hangul.extending_jung_idx) {
        if (hangul.jong_idx) {
          set_hangul_jong(&cur_hangul, hangul.jong_idx);
          add_ptrn(&ptrn, cur_hangul.letter, cur_len + 1);
          cur_len = 0;
          init_hangul_none(&cur_hangul);
        } else {
          cur_len ++;
        }
      // new part
      } else {
        // the previous part hasn't been added
        if (cur_len > 0) {
          unset_hangul_jong(&cur_hangul);
          add_ptrn(&ptrn, cur_hangul.letter, cur_len);
        }

        if (hangul.jong_idx) {
          add_ptrn(&ptrn, hangul.letter, 1);
          cur_len = 0;
          init_hangul_none(&cur_hangul);
        } else {
          cur_len = 1;
          cur_hangul = hangul;
        }
      }

      is_first_letter = false;
    } else {
      if (cur_len > 0) {
        unset_hangul_jong(&cur_hangul);
        add_ptrn(&ptrn, cur_hangul.letter, cur_len);
      }

      end_ptrn(&ptrn);

      if (match_ptrn(&ptrn, L"애니멀 보이스")) {
        cmd = ANIMAL_VOICE;
      } else if (match_ptrn(&ptrn, L"노말 트래쉬")) {
        cmd = NORMAL_TRASH;
      } else if (match_ptrn(&ptrn, L"이어폰 브레이커")) {
        cmd = EARPHONE_BREAKER;
      } else if (match_ptrn(&ptrn, L"노이즈 보이")) {
        cmd = NOISE_BOY;
      } else if (match_ptrn(&ptrn, L"일루미나티")) {
        cmd = ILLUMINATI;
      } else if (match_ptrn(&ptrn, L"욱")) {
        adjust_ptrn(&ptrn, 0, -1);
        cmd = UUK;
      } else if (match_ptrn(&ptrn, L"날강두")) {
        cmd = NALGANGDU;
      } else if (match_ptrn(&ptrn, L"순두부 빌런")) {
        cmd = SUNDUBU_VILLAIN;
      } else if (match_ptrn(&ptrn, L"저는 살려주세요")) {
        cmd = JEONUN_SALRYEOJUSEYO;
      } else if (match_ptrn(&ptrn, L"크롬 운영체제")) {
        cmd = CHROME_OS;
      } else if (match_ptrn(&ptrn, L"윈도우 웹브라우저")) {
        cmd = WINDOWS_WEB_BROWSER;
      } else if (match_ptrn(&ptrn, L"제로투 추고 싶다")) {
        cmd = ZEROTWO_CHUGO_SIPDA;
      } else {
        fprintf(stderr, "현상현 수준...\n");
        run = false;
        continue;
      }

      argument = 1;
      for (i = 0; i < ptrn.idx; i++) {
        argument *= ptrn.lens[i];
      }

      switch (cmd) {
        case ANIMAL_VOICE:
          if (field.size <= field.idx + argument) {
            inc_field_up_to(&field, field.idx + argument);
          }

          field.idx += argument;

          break;

        case NORMAL_TRASH:
          if (argument > field.idx) {
            fprintf(stderr, "상현아 멍청한 티 내지 말라니까???\n");
            run = false;
          }

          field.idx -= argument;

          break;

        case EARPHONE_BREAKER:
          field.arr[field.idx] += argument;
          break;

        case NOISE_BOY:
          field.arr[field.idx] -= argument;
          break;

        case ILLUMINATI:
          for (i = 0; i < argument; i ++) {
            printf("%lc", field.arr[field.idx]);
          }
          fflush(stdout);
          break;

        case UUK:
          if (field.size <= field.idx + argument) {
            inc_field_up_to(&field, field.idx + argument);
          }

          for (i = 0; i < argument; i++) {
            field.arr[field.idx ++] = getwchar();
          }
          field.idx -= argument;

          break;

        case NALGANGDU:
          if (field.arr[field.idx]) {
            fgoline(src, argument);

            if (errno == EINVAL) {
              fprintf(stderr, "아 상현아 제발");
              run = false;
            }
          }

          break;

        case SUNDUBU_VILLAIN:
          if (!field.arr[field.idx]) {
            fgoline(src, argument);

            if (errno == EINVAL) {
              fprintf(stderr, "아 상현이랑 못 놀아주겠다\n");
              run = false;
            }
          }

          break;

        case JEONUN_SALRYEOJUSEYO:
          exit_status = argument - 1;
          run = false;
          break;

        case CHROME_OS:
          fgoline(src, argument);

          if (errno == EINVAL) {
            fprintf(stderr, "아 상현이랑 못 놀아주겠다\n");
            run = false;
          }

          break;

        case WINDOWS_WEB_BROWSER:
          if (field.size <= argument) {
            inc_field_up_to(&field, argument);
          }

          field.arr[field.idx] = field.arr[argument];

          break;

        case ZEROTWO_CHUGO_SIPDA:
          if (field.size <= argument) {
            inc_field_up_to(&field, argument);
          }

          field.arr[field.idx] = field.arr[field.idx];
          field.arr[argument] = 0;

          break;

        default:
          fprintf(stderr, "니가 그러니까 현상현인거야");
          run = false;
          break;
      }

      init_hangul_none(&cur_hangul);
      ptrn.idx = 0;
      cur_len = 0;
      is_first_letter = true;
    }

    prev_c = c;
  }

  free_field(&field);
  fclose(src);

  return exit_status;
}

void fgoline(FILE *f, const unsigned line) {
  wchar_t c;
  unsigned line_cnt;

  fseek(f, 0, SEEK_SET);

  if (line == 1) {
    return;
  }

  line_cnt = 1;
  while ((c = fgetwc(f)) != WEOF) {
    if (c == L'\n') {
      line_cnt ++;
    }

    if (line_cnt == line) {
      return;
    }
  }

  // if the target line is bigger than the number of all lines of the file
  errno = EINVAL;
}

void init_field(Field *field, const unsigned chunk_size) {
  field->chunk_size = chunk_size;
  field->size = chunk_size;
  field->idx =  0;
  field->arr = calloc(field->size, sizeof(wchar_t));
}

void free_field(Field *field) {
  free(field->arr);
}

void inc_field_up_to(Field *field, const unsigned target_size) {
  unsigned original_size;

  original_size = field->size;
  while (field->size <= target_size) {
    field->size += field->chunk_size;
  }

  field->arr = realloc(field->arr, field->size * sizeof(wchar_t));
  memset(field->arr + original_size, 0, (field->size - original_size) * sizeof(wchar_t));
}

unsigned get_extending_jung_idx(const JungIdx jung_idx) {
  JungIdx extending_jung_idx;

  switch (jung_idx) {
    case JUNG_YA_IDX:
    case JUNG_OA_IDX:
      extending_jung_idx = JUNG_A_IDX;
      break;

    case JUNG_YAI_IDX:
      extending_jung_idx = JUNG_AI_IDX;
      break;

    case JUNG_YEO_IDX:
    case JUNG_UEO_IDX:
      extending_jung_idx = JUNG_EO_IDX;
      break;

    case JUNG_YEOI_IDX:
      extending_jung_idx = JUNG_EOI_IDX;
      break;

    case JUNG_OAI_IDX:
    case JUNG_OI_IDX:
      extending_jung_idx = JUNG_AI_IDX;
      break;

    case JUNG_YO_IDX:
      extending_jung_idx = JUNG_O_IDX;
      break;

    case JUNG_UEOI_IDX:
      extending_jung_idx = JUNG_EOI_IDX;
      break;

    case JUNG_UI_IDX:
    case JUNG_EUI_IDX:
      extending_jung_idx = JUNG_I_IDX;
      break;

    case JUNG_YU_IDX:
      extending_jung_idx = JUNG_U_IDX;
      break;

    default:
      extending_jung_idx = jung_idx;
  }

  return extending_jung_idx;
}

void init_hangul_with_letter(Hangul *hangul, const wchar_t letter) {
  unsigned temp;
  hangul->letter = letter;
  hangul->hangul_idx = letter - 0xAC00;
  hangul->cho_idx = hangul->hangul_idx / 588;
  temp = hangul->hangul_idx % 588;
  hangul->jung_idx = temp / 28;
  hangul->jong_idx = temp % 28;
  hangul->extending_jung_idx = get_extending_jung_idx(hangul->jung_idx);
}

void init_hangul_with_idxs(Hangul *hangul, const ChoIdx cho_idx, const JungIdx jung_idx, const JongIdx jong_idx) {
  hangul->cho_idx = cho_idx;
  hangul->jung_idx = jung_idx;
  hangul->jong_idx = jong_idx;
  hangul->extending_jung_idx = get_extending_jung_idx(hangul->jung_idx);

  hangul->letter = ((hangul->cho_idx * 21) + hangul->jung_idx) * 28 + hangul->jong_idx + 0xAC00;
}

void init_hangul_none(Hangul *hangul) {
  hangul->cho_idx = -1;
  hangul->jung_idx = -1;
  hangul->jong_idx = -1;
  hangul->extending_jung_idx = -1;
  hangul->hangul_idx = -1;
  hangul->letter = -1;
}

void set_hangul_jong(Hangul *hangul, const unsigned jong_idx) {
  hangul->letter = hangul->letter - hangul->jong_idx + jong_idx;
  hangul->jong_idx = jong_idx;
}

void unset_hangul_jong(Hangul *hangul) {
  set_hangul_jong(hangul, 0);
}

void init_ptrn(Ptrn *ptrn) {
  ptrn->idx = 0;
  memset(ptrn->lens, 0, (9 + 1) * sizeof(unsigned));
}

void add_ptrn(Ptrn *ptrn, const wchar_t letter, const unsigned len) {
  ptrn->ptrn[ptrn->idx] = letter;
  ptrn->lens[ptrn->idx] = len;
  ptrn->idx ++;
}

void end_ptrn(Ptrn *ptrn) {
  ptrn->ptrn[ptrn->idx] = '\0';
}

void adjust_ptrn(Ptrn *ptrn, const unsigned idx, const int value) {
  ptrn->lens[idx] += value;
}

bool match_ptrn(const Ptrn *ptrn, const wchar_t *target) {
  return !wcscmp(ptrn->ptrn, target);
}

bool is_ptrn_full(const Ptrn *ptrn) {
  return ptrn->idx == 9;
}

