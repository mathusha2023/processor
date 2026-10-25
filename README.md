# Виртуальный процессор и ассемблер для него

> Реализация собственного виртуального процессора на языке С, работающего через стэк, 
а также ассемблера для создания человеко-читаемых программ

---

## Введение
Данный проект состоит из двух частей: непосредственно виртуального процессора и ассемблер-компилятора. Компилятор принимает на вход файл с [программой на ассемблере](#о-ассемблере) и преобразует ее в бинарный исполняемый [файл формата .baa](#о-собственном-бинарном-формате-baa) с созданием промежуточного debug-файла формата .baa.txt. Процессор исполняет непосредственно бинарный файл с предварительной проверкой его целостности.

---

## Документация
В проекте настроена автоматическая генерация документации через доксиген. Документация автоматически заливается в облако через github pages при каждом пуше. Ознакомиться с ней можно [здесь](https://mathusha2023.github.io/processor/). Для самостоятельной генерации документации прямо в папке рабочего проекта используйте
```
doxygen
```

---

## Feature list
- реализован подсчет хэша для проверки целостности исполняемого файла

- реализована видеопамять 

- реализована команда для вывода содержимого видеопамяти на устройство вывода - специализированный аппаратный монитор, разработанный специально под данный процессор. в настоящее время в качестве его используется консоль))

- все тудушки перенесы в фичлист (сюда) для привлечения инвесторов к данному стартапу ( ˘ ³˘)♥

- сделано бэд аппле (пока просто аппле в виде кружка)

- реализована документация через doxygen + настроен деплой через github actions

---

## О процессоре
Процессор во многом повторяет архитектуру настоящих однокристальных процессоров. Он работает на стэке, имеет 8 регистров и непосредственный доступ к оперативной памяти (в данной реализации - просто по указателю на ее начало). Также для удобства сделан отдельный стэк адресов возврата вместо хранения этих адресов непосредственно в аппаратном стэке, что делает использование функций более удобным (не надо париться о том, чтобы сохранить правильное значение перехода на вершине стэка). Данные в стэке хранятся с фиксированной точностью(все значения для хранения умножаются на константу `DELTA` = 1000)

---

## О ассемблере

- `PUSH` — кладёт значение в стек. Варианты аргумента:
  - `PUSH int` — целое число;
  - `PUSH reg` — значение регистра;
  - `PUSH [int]` — значение из памяти по индексу;
  - `PUSH [reg]` — значение из памяти по индексу, взятому из регистра.

- `POP` — снимает верхнее значение стека и записывает его...
  - `POP reg` — в регистр;
  - `POP [int]` — в память по индексу;
  - `POP [reg]` — в память по индексу, взятому из регистра.


- `ADD` — снимает два верхних значения, складывает и кладёт результат в стек.
- `SUB` — снимает `A` и `B`, кладёт `B − A`; `B` — значение, положенное раньше.
- `MUL` — снимает два верхних значения, перемножает и кладёт результат в стек.
- `DIV` — снимает `A` и `B`, кладёт `B / A`; `B` — значение, положенное раньше.
- `ABS` — заменяет верхнее значение стека его модулем.
- `SQR` - заменяет верхнее значение стека его квадратом.
- `SQRT` — заменяет верхнее значение стека его квадратным корнем.


- `INC reg` — увеличивает значение в регистре на 1.
- `DEC reg` — уменьшает значение в регистре на 1.


- `HLT` — выход из программы.
- `JMP :label` — безусловный переход на метку.
- `JA :label` — снимает `A` и `B` (`B` положено раньше) и делает JMP, если `B` больше, чем `A`.
- `JB :label` — ... если `B` меньше, чем `A`.
- `JAE :label` — ... если `B` больше или равно `A`.
- `JBE :label` — ... если `B` меньше или равно `A`.
- `JE :label` — ... если `B` равно `A`.
- `JNE :label` — ... если `B`  не равно `A`.
- `CALL :label` — вызов "функции": сохраняет адрес возврата и переходит на метку.
- `RET` — возврат из подпрограммы по сохранённому адресу.


- `IN` — читает число из ввода и кладёт его в стек.
- `OUT` — снимает верхнее значение стека и выводит его как число.
- `OUTC` — снимает верхнее значение стека и выводит его как символ.
- `RND` — кладёт в стек случайное число.

- `DMP` - выводит в консоль пробел за каждый ноль в оперативке и `@` за любое другое число, вывод происходит в виде квадрата (печать данных ОЗУ в виде матрицы n на n)


Метки имеют следующий формат: `:label_name` (label_name - любая строка)

Комментарии в коде оставляются при помощи символа `;` (`; это комментарий!`)

---

## О собственном бинарном формате (.baa)
Процессор получает исполняемый файл формата .baa. Формат имеет следующую структуру:
- Первые 4 байта - Magic numbers, т.е. подпись компилятора
- Следующие 2 байта - номер версии (в формате 1.2, каждый байт это одно из этих значений)
- Далее 4 байта - длина кода в формате беззнакового целого
- Следующие 4 байта - хэш массива кода
- Остальные данные - сам массив с исполняемым кодом программы

---

## Автогенерация файлов с наборами всех команд для ассемблера и процессора
В связи с огромным количеством команд было решено хранить их набор в файле `commands.json`, а все файлы, в которых эти списки используются (`command_switch.h` и `commands_table.h` в процессоре и `asm_table.h` в ассемблере) генерировать этим скриптом.

Запуск генерации (из корня проекта): 

```
python3 generate_tables.py
```

После этого все файлы появятся в нужных папках

---

## Запуск 
Склонируйте репозиторий с проектом 

```
git clone https://github.com/mathusha2023/processor
```

Перейдите в папку с копией проекта

```
cd processor
```

Скомпилируйте ассемблер и процессор

``` 
cd assembler
chmod +x ./build.sh
./build.sh

cd ../vm
chmod +x ./build.sh
./build.sh
cd ..
```

Ассемблирование запускается командой

```
assembler/asm <path-to-file>
```

После этого скомпилированный файл (res.baa) запускается с помощью

```
vm/vm <path-to-compile-file/res.baa>
```

---

## Примеры программ на ассемблере
<details>
<summary>Факториал числа</summary>

```
    ; Программа вычисляющая факториал числа
    
    CALL :enter_your_number_text
    IN ; число факториал которого мы берем
    POP AX

    ; если число меньше 0, то выходим из проги
    PUSH AX
    PUSH 0
    JB :incorrect_input_exit

    CALL :factorial
    CALL :your_answer_text
    PUSH DX
    OUT

    JMP :exit

; Ниже идут функции

; в AX передаем текущее значение n, 
; в конце работы результат будет в DX
:factorial 
        PUSH AX 
        PUSH 1
        JBE :base_factorial

        PUSH AX ; сохранить значение n в стеке
        DEC AX

        CALL :factorial

        POP AX ; вернуть AX ее значение, которое было передано

        PUSH AX
        PUSH DX 
        MUL
        POP DX 

        RET

    :base_factorial
        PUSH 1
        POP DX
        RET

:incorrect_input_exit
    CALL :error_text
    JMP :exit

:exit 
    HLT

:enter_your_number_text
    PUSH 69
    OUTC
    PUSH 110
    OUTC
    PUSH 116
    OUTC
    PUSH 101
    OUTC
    PUSH 114
    OUTC
    PUSH 32
    OUTC
    PUSH 121
    OUTC
    PUSH 111
    OUTC
    PUSH 117
    OUTC
    PUSH 114
    OUTC
    PUSH 32
    OUTC
    PUSH 110
    OUTC
    PUSH 117
    OUTC
    PUSH 109
    OUTC
    PUSH 98
    OUTC
    PUSH 101
    OUTC
    PUSH 114
    OUTC
    PUSH 58
    OUTC
    PUSH 10
    OUTC
    RET

:error_text 
    PUSH 69
    OUTC
    PUSH 114
    OUTC
    PUSH 114
    OUTC
    PUSH 111
    OUTC
    PUSH 114
    OUTC
    PUSH 58
    OUTC
    PUSH 32
    OUTC
    PUSH 121
    OUTC
    PUSH 111
    OUTC
    PUSH 117
    OUTC
    PUSH 114
    OUTC
    PUSH 32
    OUTC
    PUSH 110
    OUTC
    PUSH 117
    OUTC
    PUSH 109
    OUTC
    PUSH 98
    OUTC
    PUSH 101
    OUTC
    PUSH 114
    OUTC
    PUSH 32
    OUTC
    PUSH 109
    OUTC
    PUSH 117
    OUTC
    PUSH 115
    OUTC
    PUSH 116
    OUTC
    PUSH 32
    OUTC
    PUSH 98
    OUTC
    PUSH 101
    OUTC
    PUSH 32
    OUTC
    PUSH 112
    OUTC
    PUSH 111
    OUTC
    PUSH 115
    OUTC
    PUSH 105
    OUTC
    PUSH 116
    OUTC
    PUSH 105
    OUTC
    PUSH 118
    OUTC
    PUSH 101
    OUTC
    PUSH 32
    OUTC
    PUSH 111
    OUTC
    PUSH 114
    OUTC
    PUSH 32
    OUTC
    PUSH 48
    OUTC
    PUSH 10
    OUTC
    RET

:your_answer_text
    PUSH 89
    OUTC
    PUSH 111
    OUTC
    PUSH 117
    OUTC
    PUSH 114
    OUTC
    PUSH 32
    OUTC
    PUSH 97
    OUTC
    PUSH 110
    OUTC
    PUSH 115
    OUTC
    PUSH 119
    OUTC
    PUSH 101
    OUTC
    PUSH 114
    OUTC
    PUSH 58
    OUTC
    PUSH 32
    OUTC
    RET

```

</details>

<details>
<summary>Решение квадратного уравнения</summary>

```
    ; Программа решающая квадратное уравнение по заданным коэффициентам
    
    CALL :write_a_coeff_hint
    IN
    POP AX

    CALL :write_b_coeff_hint
    IN 
    POP BX

    CALL :write_c_coeff_hint
    IN 
    POP CX

    PUSH AX 
    PUSH 0
    JE :solve_linear_zalupa

    CALL :count_d

    PUSH DX 
    PUSH 0
    JB :d<0bl

    PUSH DX 
    PUSH 0
    JE :d=0bl 

    JMP :d>0bl    

    JMP :exit

:count_d
    PUSH BX 
    PUSH BX 
    MUL
    PUSH 4
    PUSH AX 
    PUSH CX 
    MUL 
    MUL 
    SUB
    POP DX
    RET

:d<0bl 
    CALL :write_roots_count_hint
    PUSH 0
    OUT
    JMP :exit

:d=0bl
    CALL :write_roots_count_hint
    PUSH 1
    OUT 

    PUSH BX 
    PUSH -1
    MUL 
    PUSH AX 
    PUSH 2
    MUL
    DIV

    CALL :write_root_hint
    OUT

    JMP :exit

:d>0bl 
    CALL :write_roots_count_hint
    PUSH 2
    OUT 

    PUSH DX 
    SQRT
    POP GX

    PUSH BX 
    PUSH -1
    MUL 
    PUSH GX 
    SUB 
    PUSH 2
    PUSH AX 
    MUL 
    DIV

    PUSH BX 
    PUSH -1
    MUL 
    PUSH GX 
    ADD
    PUSH 2
    PUSH AX 
    MUL 
    DIV

    CALL :write_roots_hint
    OUT
    OUT 

    JMP :exit


:solve_linear_zalupa
        PUSH BX 
        PUSH 0
        JE :b_is_0

        CALL :write_roots_count_hint
        PUSH 1
        OUT 

        PUSH -1
        PUSH CX 
        MUL 
        PUSH BX 
        DIV

        CALL :write_root_hint
        OUT

        JMP :exit

    :b_is_0
        PUSH CX 
        PUSH 0
        JE :c_is_0
        JMP :c_isnt_0

    :c_is_0 
        CALL :write_roots_count_hint
        CALL :write_infinity
        JMP :exit

    :c_isnt_0
        CALL :write_roots_count_hint
        PUSH 0
        OUT
        JMP :exit
        
:exit
    HLT

:write_a_coeff_hint
    PUSH 69
    OUTC
    PUSH 110
    OUTC
    PUSH 116
    OUTC
    PUSH 101
    OUTC
    PUSH 114
    OUTC
    PUSH 32
    OUTC
    PUSH 97
    OUTC
    PUSH 32
    OUTC
    PUSH 99
    OUTC
    PUSH 111
    OUTC
    PUSH 101
    OUTC
    PUSH 102
    OUTC
    PUSH 102
    OUTC
    PUSH 58
    OUTC
    PUSH 32
    OUTC
    PUSH 10
    OUTC
    RET

:write_b_coeff_hint
    PUSH 69
    OUTC
    PUSH 110
    OUTC
    PUSH 116
    OUTC
    PUSH 101
    OUTC
    PUSH 114
    OUTC
    PUSH 32
    OUTC
    PUSH 98
    OUTC
    PUSH 32
    OUTC
    PUSH 99
    OUTC
    PUSH 111
    OUTC
    PUSH 101
    OUTC
    PUSH 102
    OUTC
    PUSH 102
    OUTC
    PUSH 58
    OUTC
    PUSH 32
    OUTC
    PUSH 10
    OUTC
    RET

:write_c_coeff_hint
    PUSH 69
    OUTC
    PUSH 110
    OUTC
    PUSH 116
    OUTC
    PUSH 101
    OUTC
    PUSH 114
    OUTC
    PUSH 32
    OUTC
    PUSH 99
    OUTC
    PUSH 32
    OUTC
    PUSH 99
    OUTC
    PUSH 111
    OUTC
    PUSH 101
    OUTC
    PUSH 102
    OUTC
    PUSH 102
    OUTC
    PUSH 58
    OUTC
    PUSH 32
    OUTC
    PUSH 10
    OUTC
    RET

:write_roots_count_hint
    PUSH 82
    OUTC
    PUSH 111
    OUTC
    PUSH 111
    OUTC
    PUSH 116
    OUTC
    PUSH 115
    OUTC
    PUSH 32
    OUTC
    PUSH 99
    OUTC
    PUSH 111
    OUTC
    PUSH 117
    OUTC
    PUSH 110
    OUTC
    PUSH 116
    OUTC
    PUSH 58
    OUTC
    PUSH 32
    OUTC
    RET

:write_roots_hint
    PUSH 82
    OUTC
    PUSH 111
    OUTC
    PUSH 111
    OUTC
    PUSH 116
    OUTC
    PUSH 115
    OUTC
    PUSH 58
    OUTC
    PUSH 10
    OUTC
    RET

:write_root_hint
    PUSH 82
    OUTC
    PUSH 111
    OUTC
    PUSH 111
    OUTC
    PUSH 116
    OUTC
    PUSH 58
    OUTC
    PUSH 32
    OUTC
    RET

:write_infinity
    PUSH 105
    OUTC
    PUSH 110
    OUTC
    PUSH 102
    OUTC
    PUSH 105
    OUTC
    PUSH 110
    OUTC
    PUSH 105
    OUTC
    PUSH 116
    OUTC
    PUSH 121
    OUTC
    PUSH 10
    OUTC
    RET

```

</details>

Другие примеры реализованных программ можно найти [здесь](./test_programs/)

---

## Связь со мной

Я всегда рад отзывам, предложениям, общению и денюжке. Свяжитесь со мной, если хотите предложить мне один из этих пунктов: [гав](https://t.me/mathusha2023)
