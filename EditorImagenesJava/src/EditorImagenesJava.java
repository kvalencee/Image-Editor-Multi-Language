import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;
import java.io.File;
import java.io.IOException;
import java.util.Scanner;

// Clase para manejar operaciones básicas con imágenes
class Imagen {
    private BufferedImage imagen;
    private int ancho;
    private int alto;
    private int[][] pixelesR;
    private int[][] pixelesG;
    private int[][] pixelesB;
    private String nombreOriginal;
    private String carpetaBase;

    public Imagen() {
        this.imagen = null;
        this.ancho = 0;
        this.alto = 0;
        this.nombreOriginal = "";
        this.carpetaBase = System.getProperty("user.dir");
    }

    public boolean cargarImagen(String rutaArchivo) {
        try {
            File archivo = new File(rutaArchivo);
            imagen = ImageIO.read(archivo);

            if (imagen == null) {
                System.out.println("Error: No se pudo cargar la imagen");
                return false;
            }

            ancho = imagen.getWidth();
            alto = imagen.getHeight();

            // Extraer nombre original sin extensión
            String nombreCompleto = archivo.getName();
            int puntoIndex = nombreCompleto.lastIndexOf('.');
            nombreOriginal = (puntoIndex > 0) ? nombreCompleto.substring(0, puntoIndex) : nombreCompleto;

            // Inicializar matrices de píxeles
            pixelesR = new int[alto][ancho];
            pixelesG = new int[alto][ancho];
            pixelesB = new int[alto][ancho];

            // Extraer componentes RGB de cada píxel
            for (int y = 0; y < alto; y++) {
                for (int x = 0; x < ancho; x++) {
                    int rgb = imagen.getRGB(x, y);
                    pixelesR[y][x] = (rgb >> 16) & 0xFF;
                    pixelesG[y][x] = (rgb >> 8) & 0xFF;
                    pixelesB[y][x] = rgb & 0xFF;
                }
            }

            System.out.println("Imagen cargada: " + ancho + "x" + alto + " píxeles");
            return true;
        } catch (IOException e) {
            System.out.println("Error al cargar imagen: " + e.getMessage());
            return false;
        }
    }

    public boolean guardarImagenEnCarpeta(String nombreFiltro, String sufijo) {
        try {
            // Crear estructura de carpetas: filtro/java/
            File carpetaFiltro = new File(carpetaBase, nombreFiltro);
            File carpetaJava = new File(carpetaFiltro, "java");

            // Crear carpetas si no existen
            if (!carpetaJava.exists()) {
                carpetaJava.mkdirs();
            }

            // Generar nombre del archivo
            String nombreArchivo = nombreOriginal + "_" + nombreFiltro + sufijo + ".jpg";
            File archivoDestino = new File(carpetaJava, nombreArchivo);

            // Crear imagen desde matriz de píxeles
            BufferedImage imagenNueva = new BufferedImage(ancho, alto, BufferedImage.TYPE_INT_RGB);

            for (int y = 0; y < alto; y++) {
                for (int x = 0; x < ancho; x++) {
                    int r = Math.max(0, Math.min(255, pixelesR[y][x]));
                    int g = Math.max(0, Math.min(255, pixelesG[y][x]));
                    int b = Math.max(0, Math.min(255, pixelesB[y][x]));

                    int rgb = (r << 16) | (g << 8) | b;
                    imagenNueva.setRGB(x, y, rgb);
                }
            }

            ImageIO.write(imagenNueva, "jpg", archivoDestino);
            System.out.println("Imagen guardada en: " + archivoDestino.getAbsolutePath());
            return true;
        } catch (IOException e) {
            System.out.println("Error al guardar imagen: " + e.getMessage());
            return false;
        }
    }

    public boolean guardarImagen(String rutaArchivo) {
        try {
            BufferedImage imagenNueva = new BufferedImage(ancho, alto, BufferedImage.TYPE_INT_RGB);

            for (int y = 0; y < alto; y++) {
                for (int x = 0; x < ancho; x++) {
                    int r = Math.max(0, Math.min(255, pixelesR[y][x]));
                    int g = Math.max(0, Math.min(255, pixelesG[y][x]));
                    int b = Math.max(0, Math.min(255, pixelesB[y][x]));

                    int rgb = (r << 16) | (g << 8) | b;
                    imagenNueva.setRGB(x, y, rgb);
                }
            }

            String formato = rutaArchivo.substring(rutaArchivo.lastIndexOf('.') + 1);
            File archivo = new File(rutaArchivo);
            ImageIO.write(imagenNueva, formato, archivo);

            System.out.println("Imagen guardada en: " + rutaArchivo);
            return true;
        } catch (IOException e) {
            System.out.println("Error al guardar imagen: " + e.getMessage());
            return false;
        }
    }

    public int[] obtenerPixel(int x, int y) {
        if (x >= 0 && x < ancho && y >= 0 && y < alto) {
            return new int[]{pixelesR[y][x], pixelesG[y][x], pixelesB[y][x]};
        }
        return new int[]{0, 0, 0};
    }

    public void establecerPixel(int x, int y, int r, int g, int b) {
        if (x >= 0 && x < ancho && y >= 0 && y < alto) {
            pixelesR[y][x] = r;
            pixelesG[y][x] = g;
            pixelesB[y][x] = b;
        }
    }

    public int getAncho() { return ancho; }
    public int getAlto() { return alto; }
    public boolean tieneImagen() { return imagen != null; }
    public String getNombreOriginal() { return nombreOriginal; }
}

// Interfaz base para todos los filtros
interface Filtro {
    void aplicar(Imagen imagen);
}

// Filtro para convertir imagen a escala de grises
class FiltroGrises implements Filtro {
    @Override
    public void aplicar(Imagen imagen) {
        System.out.println("Aplicando filtro de escala de grises...");

        for (int y = 0; y < imagen.getAlto(); y++) {
            for (int x = 0; x < imagen.getAncho(); x++) {
                int[] rgb = imagen.obtenerPixel(x, y);
                int r = rgb[0];
                int g = rgb[1];
                int b = rgb[2];

                // Fórmula estándar para escala de grises
                int gris = (int)(0.299 * r + 0.587 * g + 0.114 * b);

                imagen.establecerPixel(x, y, gris, gris, gris);
            }
        }

        System.out.println("Filtro de grises aplicado correctamente");
        // Guardar automáticamente en carpeta correspondiente
        imagen.guardarImagenEnCarpeta("grises", "");
    }
}

// Filtro para invertir los colores de la imagen
class FiltroInversion implements Filtro {
    @Override
    public void aplicar(Imagen imagen) {
        System.out.println("Aplicando filtro de inversión de colores...");

        for (int y = 0; y < imagen.getAlto(); y++) {
            for (int x = 0; x < imagen.getAncho(); x++) {
                int[] rgb = imagen.obtenerPixel(x, y);
                int r = rgb[0];
                int g = rgb[1];
                int b = rgb[2];

                // Invertir cada canal de color
                int rInv = 255 - r;
                int gInv = 255 - g;
                int bInv = 255 - b;

                imagen.establecerPixel(x, y, rInv, gInv, bInv);
            }
        }

        System.out.println("Filtro de inversión aplicado correctamente");
        // Guardar automáticamente en carpeta correspondiente
        imagen.guardarImagenEnCarpeta("inversion_colores", "");
    }
}

// Filtro para ajustar el brillo de la imagen
class FiltroBrillo implements Filtro {
    private int factorBrillo;

    public FiltroBrillo(int factorBrillo) {
        this.factorBrillo = factorBrillo;
    }

    @Override
    public void aplicar(Imagen imagen) {
        System.out.println("Aplicando filtro de brillo (factor: " + factorBrillo + ")...");

        for (int y = 0; y < imagen.getAlto(); y++) {
            for (int x = 0; x < imagen.getAncho(); x++) {
                int[] rgb = imagen.obtenerPixel(x, y);
                int r = rgb[0];
                int g = rgb[1];
                int b = rgb[2];

                // Ajustar brillo
                int rNuevo = Math.max(0, Math.min(255, r + factorBrillo));
                int gNuevo = Math.max(0, Math.min(255, g + factorBrillo));
                int bNuevo = Math.max(0, Math.min(255, b + factorBrillo));

                imagen.establecerPixel(x, y, rNuevo, gNuevo, bNuevo);
            }
        }

        System.out.println("Filtro de brillo aplicado correctamente");
        // Guardar automáticamente en carpeta correspondiente
        String carpeta = factorBrillo > 0 ? "brillo+50" : "brillo-50";
        imagen.guardarImagenEnCarpeta(carpeta, "");
    }
}

// Clase principal del editor de imágenes
class EditorImagenes {
    private Imagen imagen;
    private FiltroGrises filtroGrises;
    private FiltroInversion filtroInversion;
    private FiltroBrillo filtroBrilloMas;
    private FiltroBrillo filtroBrilloMenos;
    private Scanner scanner;

    public EditorImagenes() {
        imagen = new Imagen();
        filtroGrises = new FiltroGrises();
        filtroInversion = new FiltroInversion();
        filtroBrilloMas = new FiltroBrillo(50);
        filtroBrilloMenos = new FiltroBrillo(-50);
        scanner = new Scanner(System.in);
    }

    public void mostrarMenu() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("    EDITOR DE IMÁGENES - JAVA");
        System.out.println("=".repeat(50));
        System.out.println("1. Cargar imagen");
        System.out.println("2. Aplicar filtro de escala de grises");
        System.out.println("3. Aplicar filtro de inversión de colores");
        System.out.println("4. Aplicar filtro de brillo (+50)");
        System.out.println("5. Aplicar filtro de brillo (-50)");
        System.out.println("6. Aplicar múltiples filtros");
        System.out.println("7. Guardar imagen");
        System.out.println("8. Salir");
        System.out.println("=".repeat(50));
    }

    public void aplicarMultiplesFiltros() {
        if (!imagen.tieneImagen()) {
            System.out.println("Primero debe cargar una imagen");
            return;
        }

        System.out.println("\nSeleccione los filtros a aplicar (separados por comas):");
        System.out.println("1 - Escala de grises");
        System.out.println("2 - Inversión de colores");
        System.out.println("3 - Brillo (+50)");
        System.out.println("4 - Brillo (-50)");

        System.out.print("Filtros a aplicar: ");
        String seleccion = scanner.nextLine().trim();
        String[] filtrosSeleccionados = seleccion.split(",");

        // Lista para almacenar nombres de filtros aplicados
        StringBuilder filtrosAplicados = new StringBuilder();
        boolean primerFiltro = true;

        for (String filtroNum : filtrosSeleccionados) {
            filtroNum = filtroNum.trim();
            switch (filtroNum) {
                case "1":
                    // Solo aplicar sin guardar automáticamente para múltiples filtros
                    aplicarFiltroSinGuardar(filtroGrises);
                    if (!primerFiltro) filtrosAplicados.append("_");
                    filtrosAplicados.append("grises");
                    primerFiltro = false;
                    break;
                case "2":
                    aplicarFiltroSinGuardar(filtroInversion);
                    if (!primerFiltro) filtrosAplicados.append("_");
                    filtrosAplicados.append("inversion");
                    primerFiltro = false;
                    break;
                case "3":
                    aplicarFiltroSinGuardar(filtroBrilloMas);
                    if (!primerFiltro) filtrosAplicados.append("_");
                    filtrosAplicados.append("brillo+50");
                    primerFiltro = false;
                    break;
                case "4":
                    aplicarFiltroSinGuardar(filtroBrilloMenos);
                    if (!primerFiltro) filtrosAplicados.append("_");
                    filtrosAplicados.append("brillo-50");
                    primerFiltro = false;
                    break;
                default:
                    System.out.println("Filtro '" + filtroNum + "' no válido");
            }
        }

        // Guardar resultado final de múltiples filtros
        if (filtrosAplicados.length() > 0) {
            imagen.guardarImagenEnCarpeta("multiples_filtros", "_" + filtrosAplicados.toString());
        }
    }

    // Método auxiliar para aplicar filtros sin guardado automático (para múltiples filtros)
    private void aplicarFiltroSinGuardar(Filtro filtro) {
        if (filtro instanceof FiltroGrises) {
            aplicarGrisesSinGuardar();
        } else if (filtro instanceof FiltroInversion) {
            aplicarInversionSinGuardar();
        } else if (filtro instanceof FiltroBrillo) {
            FiltroBrillo fb = (FiltroBrillo) filtro;
            aplicarBrilloSinGuardar(fb);
        }
    }

    private void aplicarGrisesSinGuardar() {
        System.out.println("Aplicando filtro de escala de grises...");
        for (int y = 0; y < imagen.getAlto(); y++) {
            for (int x = 0; x < imagen.getAncho(); x++) {
                int[] rgb = imagen.obtenerPixel(x, y);
                int gris = (int)(0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2]);
                imagen.establecerPixel(x, y, gris, gris, gris);
            }
        }
        System.out.println("Filtro de grises aplicado correctamente");
    }

    private void aplicarInversionSinGuardar() {
        System.out.println("Aplicando filtro de inversión de colores...");
        for (int y = 0; y < imagen.getAlto(); y++) {
            for (int x = 0; x < imagen.getAncho(); x++) {
                int[] rgb = imagen.obtenerPixel(x, y);
                imagen.establecerPixel(x, y, 255 - rgb[0], 255 - rgb[1], 255 - rgb[2]);
            }
        }
        System.out.println("Filtro de inversión aplicado correctamente");
    }

    private void aplicarBrilloSinGuardar(FiltroBrillo filtro) {
        // Usar reflexión o pasar el factor como parámetro
        // Para simplicidad, asumiré los valores conocidos
        int factor = (filtro == filtroBrilloMas) ? 50 : -50;

        System.out.println("Aplicando filtro de brillo (factor: " + factor + ")...");
        for (int y = 0; y < imagen.getAlto(); y++) {
            for (int x = 0; x < imagen.getAncho(); x++) {
                int[] rgb = imagen.obtenerPixel(x, y);
                int rNuevo = Math.max(0, Math.min(255, rgb[0] + factor));
                int gNuevo = Math.max(0, Math.min(255, rgb[1] + factor));
                int bNuevo = Math.max(0, Math.min(255, rgb[2] + factor));
                imagen.establecerPixel(x, y, rNuevo, gNuevo, bNuevo);
            }
        }
        System.out.println("Filtro de brillo aplicado correctamente");
    }

    public void ejecutar() {
        System.out.println("Bienvenido al Editor de Imágenes");

        while (true) {
            mostrarMenu();
            System.out.print("Seleccione una opción: ");
            String opcion = scanner.nextLine().trim();

            switch (opcion) {
                case "1":
                    System.out.print("Ingrese la ruta de la imagen: ");
                    String ruta = scanner.nextLine().trim();
                    imagen.cargarImagen(ruta);
                    break;

                case "2":
                    if (imagen.tieneImagen()) {
                        filtroGrises.aplicar(imagen);
                    } else {
                        System.out.println("Primero debe cargar una imagen");
                    }
                    break;

                case "3":
                    if (imagen.tieneImagen()) {
                        filtroInversion.aplicar(imagen);
                    } else {
                        System.out.println("Primero debe cargar una imagen");
                    }
                    break;

                case "4":
                    if (imagen.tieneImagen()) {
                        filtroBrilloMas.aplicar(imagen);
                    } else {
                        System.out.println("Primero debe cargar una imagen");
                    }
                    break;

                case "5":
                    if (imagen.tieneImagen()) {
                        filtroBrilloMenos.aplicar(imagen);
                    } else {
                        System.out.println("Primero debe cargar una imagen");
                    }
                    break;

                case "6":
                    aplicarMultiplesFiltros();
                    break;

                case "7":
                    if (imagen.tieneImagen()) {
                        System.out.print("Ingrese la ruta para guardar: ");
                        String rutaGuardar = scanner.nextLine().trim();
                        imagen.guardarImagen(rutaGuardar);
                    } else {
                        System.out.println("No hay imagen para guardar");
                    }
                    break;

                case "8":
                    System.out.println("¡Gracias por usar el Editor de Imágenes!");
                    return;

                default:
                    System.out.println("Opción no válida");
            }
        }
    }
}

// Clase principal con método main
public class EditorImagenesJava {
    public static void main(String[] args) {
        EditorImagenes editor = new EditorImagenes();
        editor.ejecutar();
    }
}