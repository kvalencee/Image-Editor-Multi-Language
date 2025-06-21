from PIL import Image
import os
from abc import ABC, abstractmethod


class Imagen:
    """Clase para manejar operaciones básicas con imágenes"""

    def __init__(self):
        self.imagen = None
        self.ancho = 0
        self.alto = 0
        self.pixeles = []
        self.nombre_original = ""
        self.carpeta_base = os.path.dirname(os.path.abspath(__file__))

    def cargar_imagen(self, ruta_archivo):
        """Carga una imagen desde un archivo"""
        try:
            self.imagen = Image.open(ruta_archivo)
            self.imagen = self.imagen.convert('RGB')  # Asegurar formato RGB
            self.ancho, self.alto = self.imagen.size

            # Guardar nombre original sin extensión
            self.nombre_original = os.path.splitext(os.path.basename(ruta_archivo))[0]

            # Convertir a matriz de píxeles para manipulación manual
            self.pixeles = []
            for y in range(self.alto):
                fila = []
                for x in range(self.ancho):
                    r, g, b = self.imagen.getpixel((x, y))
                    fila.append((r, g, b))
                self.pixeles.append(fila)

            print(f"Imagen cargada: {self.ancho}x{self.alto} píxeles")
            return True
        except Exception as e:
            print(f"Error al cargar imagen: {e}")
            return False

    def guardar_imagen_en_carpeta(self, nombre_filtro, sufijo=""):
        """Guarda la imagen en la carpeta correspondiente al filtro aplicado"""
        try:
            # Crear estructura de carpetas: filtro/python/
            carpeta_filtro = os.path.join(self.carpeta_base, nombre_filtro)
            carpeta_python = os.path.join(carpeta_filtro, "python")

            # Crear carpetas si no existen
            os.makedirs(carpeta_python, exist_ok=True)

            # Generar nombre del archivo
            nombre_archivo = f"{self.nombre_original}_{nombre_filtro}{sufijo}.jpg"
            ruta_completa = os.path.join(carpeta_python, nombre_archivo)

            # Crear imagen desde matriz de píxeles
            imagen_nueva = Image.new('RGB', (self.ancho, self.alto))

            for y in range(self.alto):
                for x in range(self.ancho):
                    r, g, b = self.pixeles[y][x]
                    # Asegurar que los valores estén en rango válido
                    r = max(0, min(255, int(r)))
                    g = max(0, min(255, int(g)))
                    b = max(0, min(255, int(b)))
                    imagen_nueva.putpixel((x, y), (r, g, b))

            imagen_nueva.save(ruta_completa)
            print(f"Imagen guardada en: {ruta_completa}")
            return True
        except Exception as e:
            print(f"Error al guardar imagen: {e}")
            return False

    def guardar_imagen(self, ruta_archivo):
        """Guarda la imagen actual en un archivo (método original mantenido)"""
        try:
            # Crear imagen desde matriz de píxeles
            imagen_nueva = Image.new('RGB', (self.ancho, self.alto))

            for y in range(self.alto):
                for x in range(self.ancho):
                    r, g, b = self.pixeles[y][x]
                    # Asegurar que los valores estén en rango válido
                    r = max(0, min(255, int(r)))
                    g = max(0, min(255, int(g)))
                    b = max(0, min(255, int(b)))
                    imagen_nueva.putpixel((x, y), (r, g, b))

            imagen_nueva.save(ruta_archivo)
            print(f"Imagen guardada en: {ruta_archivo}")
            return True
        except Exception as e:
            print(f"Error al guardar imagen: {e}")
            return False

    def obtener_pixel(self, x, y):
        """Obtiene el valor RGB de un píxel"""
        if 0 <= x < self.ancho and 0 <= y < self.alto:
            return self.pixeles[y][x]
        return (0, 0, 0)

    def establecer_pixel(self, x, y, r, g, b):
        """Establece el valor RGB de un píxel"""
        if 0 <= x < self.ancho and 0 <= y < self.alto:
            self.pixeles[y][x] = (r, g, b)


class Filtro(ABC):
    """Clase abstracta base para todos los filtros"""

    @abstractmethod
    def aplicar(self, imagen):
        """Método abstracto que debe implementar cada filtro"""
        pass


class FiltroGrises(Filtro):
    """Filtro para convertir imagen a escala de grises"""

    def aplicar(self, imagen):
        """Aplica filtro de escala de grises usando fórmula estándar"""
        print("Aplicando filtro de escala de grises...")

        for y in range(imagen.alto):
            for x in range(imagen.ancho):
                r, g, b = imagen.obtener_pixel(x, y)

                # Fórmula estándar para escala de grises
                gris = int(0.299 * r + 0.587 * g + 0.114 * b)

                imagen.establecer_pixel(x, y, gris, gris, gris)

        print("Filtro de grises aplicado correctamente")
        # Guardar automáticamente en carpeta correspondiente
        imagen.guardar_imagen_en_carpeta("grises")


class FiltroInversion(Filtro):
    """Filtro para invertir los colores de la imagen"""

    def aplicar(self, imagen):
        """Aplica filtro de inversión de colores"""
        print("Aplicando filtro de inversión de colores...")

        for y in range(imagen.alto):
            for x in range(imagen.ancho):
                r, g, b = imagen.obtener_pixel(x, y)

                # Invertir cada canal de color
                r_inv = 255 - r
                g_inv = 255 - g
                b_inv = 255 - b

                imagen.establecer_pixel(x, y, r_inv, g_inv, b_inv)

        print("Filtro de inversión aplicado correctamente")
        # Guardar automáticamente en carpeta correspondiente
        imagen.guardar_imagen_en_carpeta("inversion_colores")


class FiltroBrillo(Filtro):
    """Filtro para ajustar el brillo de la imagen"""

    def __init__(self, factor_brillo=50):
        self.factor_brillo = factor_brillo

    def aplicar(self, imagen):
        """Aplica ajuste de brillo"""
        print(f"Aplicando filtro de brillo (factor: {self.factor_brillo})...")

        for y in range(imagen.alto):
            for x in range(imagen.ancho):
                r, g, b = imagen.obtener_pixel(x, y)

                # Ajustar brillo
                r_nuevo = max(0, min(255, r + self.factor_brillo))
                g_nuevo = max(0, min(255, g + self.factor_brillo))
                b_nuevo = max(0, min(255, b + self.factor_brillo))

                imagen.establecer_pixel(x, y, r_nuevo, g_nuevo, b_nuevo)

        print("Filtro de brillo aplicado correctamente")
        # Guardar automáticamente en carpeta correspondiente
        carpeta = "brillo+50" if self.factor_brillo > 0 else "brillo-50"
        imagen.guardar_imagen_en_carpeta(carpeta)


class EditorImagenes:
    """Clase principal del editor de imágenes"""

    def __init__(self):
        self.imagen = Imagen()
        self.filtros_disponibles = {
            '1': FiltroGrises(),
            '2': FiltroInversion(),
            '3': FiltroBrillo(50),
            '4': FiltroBrillo(-50)
        }

    def mostrar_menu(self):
        """Muestra el menú principal"""
        print("\n" + "=" * 50)
        print("    EDITOR DE IMÁGENES - PYTHON")
        print("=" * 50)
        print("1. Cargar imagen")
        print("2. Aplicar filtro de escala de grises")
        print("3. Aplicar filtro de inversión de colores")
        print("4. Aplicar filtro de brillo (+50)")
        print("5. Aplicar filtro de brillo (-50)")
        print("6. Aplicar múltiples filtros")
        print("7. Guardar imagen")
        print("8. Salir")
        print("=" * 50)

    def aplicar_multiples_filtros(self):
        """Permite aplicar múltiples filtros en cadena"""
        if not self.imagen.pixeles:
            print("Primero debe cargar una imagen")
            return

        print("\nSeleccione los filtros a aplicar (separados por comas):")
        print("1 - Escala de grises")
        print("2 - Inversión de colores")
        print("3 - Brillo (+50)")
        print("4 - Brillo (-50)")

        seleccion = input("Filtros a aplicar: ").strip()
        filtros_seleccionados = [f.strip() for f in seleccion.split(',')]

        # Lista para almacenar nombres de filtros aplicados
        filtros_aplicados = []

        for filtro_num in filtros_seleccionados:
            if filtro_num == '1':
                self.filtros_disponibles['1'].aplicar(self.imagen)
                filtros_aplicados.append("grises")
            elif filtro_num == '2':
                self.filtros_disponibles['2'].aplicar(self.imagen)
                filtros_aplicados.append("inversion")
            elif filtro_num == '3':
                self.filtros_disponibles['3'].aplicar(self.imagen)
                filtros_aplicados.append("brillo+50")
            elif filtro_num == '4':
                self.filtros_disponibles['4'].aplicar(self.imagen)
                filtros_aplicados.append("brillo-50")
            else:
                print(f"Filtro '{filtro_num}' no válido")

        # Guardar resultado final de múltiples filtros
        if filtros_aplicados:
            nombre_combinado = "_".join(filtros_aplicados)
            self.imagen.guardar_imagen_en_carpeta("multiples_filtros", f"_{nombre_combinado}")

    def ejecutar(self):
        """Ejecuta el programa principal"""
        print("Bienvenido al Editor de Imágenes")

        while True:
            self.mostrar_menu()
            opcion = input("Seleccione una opción: ").strip()

            if opcion == '1':
                ruta = input("Ingrese la ruta de la imagen: ").strip()
                self.imagen.cargar_imagen(ruta)

            elif opcion == '2':
                if self.imagen.pixeles:
                    self.filtros_disponibles['1'].aplicar(self.imagen)
                else:
                    print("Primero debe cargar una imagen")

            elif opcion == '3':
                if self.imagen.pixeles:
                    self.filtros_disponibles['2'].aplicar(self.imagen)
                else:
                    print("Primero debe cargar una imagen")

            elif opcion == '4':
                if self.imagen.pixeles:
                    self.filtros_disponibles['3'].aplicar(self.imagen)
                else:
                    print("Primero debe cargar una imagen")

            elif opcion == '5':
                if self.imagen.pixeles:
                    self.filtros_disponibles['4'].aplicar(self.imagen)
                else:
                    print("Primero debe cargar una imagen")

            elif opcion == '6':
                self.aplicar_multiples_filtros()

            elif opcion == '7':
                if self.imagen.pixeles:
                    ruta = input("Ingrese la ruta para guardar: ").strip()
                    self.imagen.guardar_imagen(ruta)
                else:
                    print("No hay imagen para guardar")

            elif opcion == '8':
                print("¡Gracias por usar el Editor de Imágenes!")
                break

            else:
                print("Opción no válida")


# Función principal
def main():
    editor = EditorImagenes()
    editor.ejecutar()


if __name__ == "__main__":
    main()