#!/usr/bin/env bash
# Compares ./pipex with the real shell: same output file and same exit status.
cd "$(dirname "$0")/.." || exit 1
make -s bonus > /dev/null || exit 1
PIPEX="$PWD/pipex"
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
cd "$T" || exit 1
printf 'hello world\nfoo bar\nhello 42\nlast line\n' > in

PASS=0
FAIL=0
result() {  # result <name> <ok>
    if [ "$2" = 1 ]; then PASS=$((PASS + 1)); printf '  \033[32mPASS\033[0m  %s\n' "$1"
    else FAIL=$((FAIL + 1)); printf '  \033[31mFAIL\033[0m  %s\n' "$1"; fi
}
# same <name> <shell command writing to OUT> <pipex args, last one is OUT>
same() {
    local name="$1" shell="$2"; shift 2
    rm -f out_s out_p
    bash -c "${shell//OUT/out_s}" 2> /dev/null; local s=$?
    local args=("$@"); args[${#args[@]}-1]=out_p
    "$PIPEX" "${args[@]}" 2> /dev/null < /dev/null; local p=$?
    if [ "$s" = "$p" ] && cmp -s out_s out_p; then result "$name" 1
    else result "$name (shell: $s, pipex: $p)" 0; fi
}

echo "mandatory"
same "cat | wc -l"              '< in cat | wc -l > OUT'                 in "cat" "wc -l" OUT
same "grep | wc -w"             '< in grep hello | wc -w > OUT'          in "grep hello" "wc -w" OUT
same "quotes in a command"      "< in awk '{print \$1}' | sort -u > OUT" in "awk '{print \$1}'" "sort -u" OUT
same "absolute paths"           '< in /bin/cat | /usr/bin/wc -c > OUT'   in "/bin/cat" "/usr/bin/wc -c" OUT
same "missing infile"           '< nofile cat | wc -l > OUT'             nofile "cat" "wc -l" OUT
same "unknown 1st command"      '< in nosuchcmd | wc -l > OUT'           in "nosuchcmd" "wc -l" OUT
same "unknown 2nd command"      '< in cat | nosuchcmd > OUT'             in "cat" "nosuchcmd" OUT
same "empty command"            '< in cat | "" > OUT'                    in "cat" "" OUT
same "exit status of the last"  '< in cat | false > OUT'                 in "cat" "false" OUT

echo "bonus"
same "4 commands"               '< in cat | grep hello | tr a-z A-Z | wc -l > OUT' in "cat" "grep hello" "tr a-z A-Z" "wc -l" OUT
rm -f out_s out_p; echo old > out_s; echo old > out_p
printf 'cat << END | tr a-z A-Z >> out_s\nabc\ndef\nEND\n' | bash
printf 'abc\ndef\nEND\n' | "$PIPEX" here_doc END "cat" "tr a-z A-Z" out_p > /dev/null
result "here_doc appends (>>)" "$(cmp -s out_s out_p && echo 1)"

echo "errors"
touch locked && chmod 000 locked
"$PIPEX" in "cat" "wc -l" locked 2> /dev/null; s=$?
[ "$(id -u)" = 0 ] && s=1     # root can write anyway
result "outfile not writable -> 1" "$([ $s = 1 ] && echo 1)"
env -i "$PIPEX" in "cat" "wc -l" out_p 2> /dev/null; s=$?
result "no PATH at all -> 127" "$([ $s = 127 ] && echo 1)"
"$PIPEX" in "cat" 2> /dev/null; s=$?
result "wrong argument count -> 1" "$([ $s = 1 ] && echo 1)"

echo
echo "  $PASS passed, $FAIL failed"
[ "$FAIL" -eq 0 ]
