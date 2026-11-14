
import tkinter as tk
from tkinter import filedialog, messagebox

# Порядок символов должен совпадать с ST7789_RUS_GetCharIndex
LETTERS = (
    "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
    "абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
)

CELL_SIZE = 48

class FontEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Редактор русского Bitmap-шрифта 5x7")
        self.root.resizable(False, False)

        self.current_index = 0

        # Каждый символ хранится как набор координат закрашенных пикселей.
        # Если символ не рисовали, он автоматически будет пустым.
        self.glyphs = {
            letter: set() for letter in LETTERS
        }

        self.mode = "paint"
        self.cells = []

        self.build_interface()
        self.update_screen()

    def build_interface(self):
        # Заголовок и текущая буква
        tk.Label(
            self.root,
            text="Рисуем символ:",
            font=("Arial", 14)
        ).pack(pady=(15, 5))

        self.letter_label = tk.Label(
            self.root,
            text="А",
            font=("Arial", 36, "bold")
        )
        self.letter_label.pack(pady=5)

        self.progress_label = tk.Label(
            self.root,
            text="1 / 66",
            font=("Arial", 11)
        )
        self.progress_label.pack(pady=5)

        # Матрица 5x7
        self.matrix = tk.Frame(self.root)
        self.matrix.pack(padx=20, pady=15)

        for row in range(7):
            cell_row = []

            for col in range(5):
                button = tk.Button(
                    self.matrix,
                    width=3,
                    height=1,
                    bg="white",
                    activebackground="lightgray",
                    relief="raised",
                    command=lambda r=row, c=col: self.pixel_click(r, c)
                )

                button.grid(
                    row=row,
                    column=col,
                    padx=3,
                    pady=3
                )

                cell_row.append(button)

            self.cells.append(cell_row)

        # Выбор режима
        tk.Label(
            self.root,
            text="Режим рисования:",
            font=("Arial", 11)
        ).pack(pady=(5, 5))

        mode_frame = tk.Frame(self.root)
        mode_frame.pack(pady=5)

        self.paint_button = tk.Button(
            mode_frame,
            text="Закрашивание",
            width=15,
            command=lambda: self.set_mode("paint")
        )
        self.paint_button.grid(row=0, column=0, padx=5)

        self.erase_button = tk.Button(
            mode_frame,
            text="Стирание",
            width=15,
            command=lambda: self.set_mode("erase")
        )
        self.erase_button.grid(row=0, column=1, padx=5)

        # Очистка текущей буквы
        tk.Button(
            self.root,
            text="Очистить букву",
            width=32,
            command=self.clear_glyph
        ).pack(pady=8)

        # Предыдущая буква
        tk.Button(
            self.root,
            text="← Предыдущая буква",
            width=32,
            command=self.previous_letter
        ).pack(pady=5)

        # Следующая буква
        tk.Button(
            self.root,
            text="Следующая буква →",
            width=32,
            command=self.next_letter
        ).pack(pady=5)

        # Экспорт
        tk.Button(
            self.root,
            text="Экспорт Bitmap",
            width=32,
            bg="#b7e4c7",
            command=self.export_bitmap
        ).pack(pady=(10, 20))

    def current_letter(self):
        return LETTERS[self.current_index]

    def pixel_click(self, row, col):
        letter = self.current_letter()
        pixels = self.glyphs[letter]
        position = (row, col)

        if self.mode == "paint":
            pixels.add(position)
        else:
            pixels.discard(position)

        self.update_matrix()

    def set_mode(self, mode):
        self.mode = mode

        self.paint_button.config(
            relief="sunken" if mode == "paint" else "raised"
        )

        self.erase_button.config(
            relief="sunken" if mode == "erase" else "raised"
        )

    def update_matrix(self):
        pixels = self.glyphs[self.current_letter()]

        for row in range(7):
            for col in range(5):
                if (row, col) in pixels:
                    self.cells[row][col].config(
                        bg="#222222",
                        activebackground="#444444"
                    )
                else:
                    self.cells[row][col].config(
                        bg="white",
                        activebackground="lightgray"
                    )

    def update_screen(self):
        letter = self.current_letter()

        self.letter_label.config(text=letter)

        self.progress_label.config(
            text=f"{self.current_index + 1} / {len(LETTERS)}"
        )

        self.update_matrix()

    def clear_glyph(self):
        letter = self.current_letter()
        self.glyphs[letter].clear()
        self.update_matrix()

    def next_letter(self):
        if self.current_index < len(LETTERS) - 1:
            self.current_index += 1
            self.update_screen()
        else:
            messagebox.showinfo(
                "Готово",
                "Это последняя буква!"
            )

    def previous_letter(self):
        if self.current_index > 0:
            self.current_index -= 1
            self.update_screen()
        else:
            messagebox.showinfo(
                "Начало",
                "Это первая буква!"
            )

    def export_bitmap(self):
        filename = filedialog.asksaveasfilename(
            title="Сохранить Bitmap-шрифт",
            initialfile="RusFont5x7_Data.h",
            defaultextension=".h",
            filetypes=[("C Header", "*.h")]
        )

        if not filename:
            return

        lines = [
            "#ifndef RUS_FONT_5X7_DATA_H",
            "#define RUS_FONT_5X7_DATA_H",
            "",
            "#include <stdint.h>",
            "",
            "static const uint8_t RusFont5x7_Data[] = {"
        ]

        for letter in LETTERS:
            pixels = self.glyphs[letter]

            # Один байт на каждый столбец.
            # Бит 0 = верхняя строка, бит 6 = нижняя строка.
            columns = []

            for col in range(5):
                value = 0

                for row in range(7):
                    if (row, col) in pixels:
                        value |= (1 << row)

                columns.append(value)

            byte_text = ", ".join(
                f"0x{value:02X}" for value in columns
            )

            lines.append(
                f"    {byte_text}, // {letter}"
            )

        lines.extend([
            "};",
            "",
            "#endif"
        ])

        try:
            with open(filename, "w", encoding="utf-8") as file:
                file.write("\n".join(lines) + "\n")

            messagebox.showinfo(
                "Экспорт завершён",
                f"Файл успешно сохранён:\n{filename}"
            )

        except OSError as error:
            messagebox.showerror(
                "Ошибка сохранения",
                str(error)
            )


if __name__ == "__main__":
    root = tk.Tk()
    app = FontEditor(root)
    root.mainloop()
