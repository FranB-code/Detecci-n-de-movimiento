#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Abrimos la cámara
    cv::VideoCapture cam(1);
    if (!cam.isOpened()) {
        std::cerr << "No se pudo abrir la cámara." << std::endl;
        return -1;
    }

    cv::Mat frameActual, frameGris, framePrevio, diferencia, umbral;
    
    std::cout<<"Pulsa q para detener"<<std::endl;

    // Esperamos a tener el primer frame como referencia
    cam >> frameActual;
    cv::cvtColor(frameActual, framePrevio, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(framePrevio, framePrevio, cv::Size(21, 21), 0);

    while (true) {
        cam >> frameActual;
        if (frameActual.empty()) break;

        // Convertimos a escala de grises y desenfocamos
        cv::cvtColor(frameActual, frameGris, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(frameGris, frameGris, cv::Size(21, 21), 0);

        // Calculamos la diferencia entre el frame actual y el anterior
        cv::absdiff(framePrevio, frameGris, diferencia);

        // Aplicamos un umbral para obtener las zonas con cambio
        cv::threshold(diferencia, umbral, 25, 255, cv::THRESH_BINARY);

        // Ampliamos las zonas detectadas
        cv::dilate(umbral, umbral, cv::Mat(), cv::Point(-1,-1), 2);

        // Encontramos los contornos del movimiento
        std::vector<std::vector<cv::Point>> contornos;
        cv::findContours(umbral, contornos, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Dibujamos los contornos en el frame original
        for (const auto& contorno : contornos) {
            if (cv::contourArea(contorno) < 500) continue;
            cv::Rect caja = cv::boundingRect(contorno);
            cv::rectangle(frameActual, caja, cv::Scalar(0, 0, 255), 2);
        }

        // Mostramos el resultado
        cv::imshow("Movimiento detectado", frameActual);

        // Actualizamos el frame previo
        framePrevio = frameGris.clone();

        // Salimos si se presiona la tecla 'q'
        if (cv::waitKey(30) == 'q') break;
    }

    cam.release();
    cv::destroyAllWindows();
    return 0;
}
