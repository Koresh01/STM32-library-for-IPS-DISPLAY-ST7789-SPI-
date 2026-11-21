# STM32-library-for-IPS-DISPLAY-ST7789-SPI-


## Возможности библиотеки.
Должен сказать что модуль этого экранчика распаян производителем под управление через SPI. Кроме того, **отсутствует пин Cheap Select**. Поэтому мой экран имеет всего лишь 7 пинов. А код библиотеки не имеет логики Cheap Select. Будьте внимательны, если вы подключаете по SPI несколько периферийных устройств!

<img width="1145" height="859" alt="IMG_7059" src="https://github.com/user-attachments/assets/b67b990b-e540-4b48-b44f-c5a1e5579052" />

## Структура папок.

В корневой папке проекта STM32 Cube IDE создать папку `Libraries` (см.рис.1)

<img width="474" height="375" alt="image" src="https://github.com/user-attachments/assets/10dec2c5-6c14-45b8-8de4-125a5c65b116" />

_рис.1 Папка Libraries_

Далее в этой папке создадим папку `ST7789` (см.рис.2). Здесь будут храниться файлы нашей библиотеки.

<img width="476" height="394" alt="image" src="https://github.com/user-attachments/assets/aa690030-925b-4cf6-9b5b-5525dac295dc" />

_рис.2 Папка ST7789_


Набор файлов Libraries/ST7789 (см.рис.3)

<img width="475" height="495" alt="image" src="https://github.com/user-attachments/assets/0a2c87f2-c4dc-4e51-9744-a727fe516d49" />

_рис.3 Содержимое папки ST7789_

## Как подключить библиотеку.

`project -> properties -> C/C++ General -> Includes -> Добавляем путь до папки ST7789`
<img width="855" height="556" alt="image" src="https://github.com/user-attachments/assets/33ec119a-5190-4ba9-a6c3-56ee540c08a1" />

`project -> properties -> C/C++ General -> Source Location -> Добавляем путь до папки Libraries`
<img width="850" height="546" alt="image" src="https://github.com/user-attachments/assets/94101aa6-c65f-48f9-9ffb-203e903664eb" />

## Полезные шпаргалки

Мы сделали библиотеку с поддержкой нескольких экземпляров устройств (multi-instance library).
<img width="1536" height="1024" alt="Создание библиотеки МУЛЬТИУСТРОЙСТВ" src="https://github.com/user-attachments/assets/b313689c-1c5d-457a-9be8-3ad2390e5dac" />
рис.4 Общая идея создания multi-устройственной библиотеки.

Русские символы кодируются двумя байтами, в отличие от однобайтовых английских.
<img width="1536" height="1024" alt="Unicode и UTF-8" src="https://github.com/user-attachments/assets/e5a22aa2-9518-4e3a-b5b3-3ae108615eff" />
рис.5 Переход из UTF-8 в Unicode и обратно.

