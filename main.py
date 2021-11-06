import sys
import re

def calculate_argument(code, user_code):
    calculated = 1

    for letter in code:
        if letter == ' ':
            user_code = user_code[1:]
            continue

        hangul_index = ord(letter) - 0xAC00
        chosung_index = hangul_index // 588
        temp = hangul_index % 588
        jungsung_index = temp // 28
        jongsung_index = temp % 28

        extending_jungsung_index = jungsung_index
        if jungsung_index == 6:
            extending_jungsung_index = 5
        elif jongsung_index == 12:
            extending_jungsung_index = 8

        pattern = chr(((chosung_index * 21) + jungsung_index) * 28 + 0xAC00)
        pattern += chr(((11 * 21) + extending_jungsung_index) * 28 + 0xAC00)
        pattern += '*'

        # there's jongsung
        if jongsung_index != 0:
            pattern += chr(((11 * 21) + extending_jungsung_index) * 28 + jongsung_index + 0xAC00)
            pattern += '|'
            pattern += letter

        pattern = '^' + pattern

        matched = re.match(pattern, user_code)
        if matched:
            matched_length = matched.span()[1]
            user_code = user_code[matched_length:]
            calculated *= matched_length

        else:
            raise ValueError(f'에휴 현상현 {pattern + " " + letter}')

    return calculated

source_file = open(sys.argv[1])

array = [0] * 3000
array_index = 0

codes = [code.strip() for code in source_file.readlines()]
code_line = 0

while code_line < len(codes):
    code = codes[code_line]

    # commands
    # 1: 애니멀 보이스: ++ptr
    # 2: 노말 트래쉬: --ptr
    # 3: 이어폰 브레이커: ++*ptr
    # 4: 노이즈 보이: --*ptr
    # 5: 일루미나티: putchar(*ptr)
    # 6: 우욱: *ptr = getchar()
    # 7: 날강두: je
    # 8: 순두부 빌런: jne
    # 9: 저는 살려주세요: exit
    if re.match(r'^애+니이*(멀|머어*얼) 보오*이+스으*$', code):
        array_index += calculate_argument("애니멀 보이스", code)

    elif re.match(r'^노오*(말|마아*알) 트으*래애*쉬이*$', code):
        array_index -= calculate_argument("노말 트래쉬", code)

    elif re.match(r'^이+어+(폰|포오*온) 브으*레에*이+커어*$', code):
        array[array_index] += calculate_argument("이어폰 브레이커", code)

    elif re.match(r'^노오*이+즈으* 보오*이+$', code):
        array[array_index] -= calculate_argument("노이즈 보이", code)

    elif re.match(r'^(일|이+일)루우*미이*나아*티이*$', code):
        print(chr(array[array_index]), end='')

    elif re.match(r'^우+욱$', code):
        array[array_index] = ord(input())

    elif re.match(r'(날|나아*알)(강|가아*앙)두우*', code):
        if array[array_index]:
            code_line = calculate_argument("날강두", code)
            continue

    elif re.match(r'^(순|수우*운)두우*부우* (빌|비이*일)(런|러어*언)$', code):
        if not array[array_index]:
            code_line = calculate_argument("순두부 빌런", code)
            continue

    elif re.match(r'^저어*(는|느으*은) (살|사아*알)려어*주우*세에*요오*$', code):
        sys.exit()

    else:
        raise ValueError(f'니가 그러니까 현상현인거야 {code_line} {code}')

    code_line += 1

