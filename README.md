PAV - P3: detección de pitch
============================

A continuación se resolveran los ejercicios propuestos para esta práctica 3 de detección del pitch.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la detección de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.
   ``` cpp
	  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {
	    //Recorremos las posiciones del vector de autocorrelación
	    for (unsigned int l = 0; l < r.size(); ++l) {
	      //Calculamos y asignamos el valor correspondiente
	      for (unsigned int n = 0; n <= x.size()-1-l; ++n){
		r[l]=r[l] + x[n]*x[n+l];
	      }
	      r[l]=(1.0/x.size())*r[l];
	    }

	    if (r[0] == 0.0F) //to avoid log() and divide zero 
	      r[0] = 1e-10; 
	  }
  ```

   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un sonido sonoro
     y su periodo de pitch; y, en otro *subplot*, se vea con claridad la autocorrelación de la señal y la
	 posición del primer máximo secundario.
	
		En la gráfica superior se puede ver la forma de onda temporal del fgragmento de audio sonoro que hemos escojido. Al tratarse de un sonido sonoro, este es periódico y, por lo tanto, para caluclar su frecuencia fundamental (pitch a nivel auditivo) tenemos que hacer la inversa del periodo: f0=1 / 0.1167-0.1094 = 137.98 Hz. Por otro lado, en el segundo gráfico se muestra la autocorrelación de la señal, donde sabemos que la distáncia entre los dos priemros máximos es el pitch: f0 = 4795-4645 =150 Hz.En ambos casos, los resultados obtenidos se asemejan y són lógicos considerando los valore smínimos y máximos que puede tomar un pitch.
	
	![Pitch_Temp_Correl](https://user-images.githubusercontent.com/61736138/77790479-361b7b80-7065-11ea-9013-b8551c268a76.png)


   * Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.
     ``` cpp

	  float PitchAnalyzer::compute_pitch(vector<float> & x, float r1norm_min, float rmaxnorm_min, float pow_min) const {
	    if (x.size() != frameLen)
	      return -1.0F;

	    //Window input frame
	    for (unsigned int i=0; i<x.size(); ++i)
	      x[i] *= window[i];

	    vector<float> r(npitch_max);

	    //Compute correlation
	    autocorrelation(x, r);

	    vector<float>::const_iterator iR = r.begin(), iRMax = iR;
	    while (*iR > 0 ){
	      iR++;
	    }

	    if(iR < r.begin() + npitch_min) iR += npitch_min;

	    iRMax = iR;
	    while(iR!=r.end()){
	      if(*iR>*iRMax){
		iRMax=iR;
	      }
	      ++iR;
	    }

	    unsigned int lag = iRMax - r.begin();

	    float pot = 10 * log10(r[0]);
	  }

     ```

   * Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.
``` cpp

 bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm, float pow_min, float r1norm_min, float rmaxnorm_min) const { 
    //Si una de les dues aproximacions de correlació i la potència superen el llindar, considerem voiced
    if ((r1norm>=r1norm_min || rmaxnorm>=rmaxnorm_min) && pot > pow_min) return false;
    return true;
  }

```
- Una vez completados los puntos anteriores, dispondrá de una primera versión del detector de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la detección de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
		(r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
		autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).
		
		En la siguiente gráfica se puede ver, empezando desde abajo, la representación temporal del señal sb050.wav (waveform), el cálculo del pitch generado por el própio Wavesurfer (Pitch Contour), la poténcia del señal, el cociente r[1]/r[0] y finalmente el cociente r[lag]/r[0]. Como se puede observar, los tres casos son adecuados para determinar si un frame es sonoro o no; así pues, la condición que hemos decidido para determinar que un fragmento es sonoro la poténcia tendrá que superar un límite y uno de los dos cocientes de autocorrelaciones también (no hace falta ambos, con uno sólo es suficiente).
		
		![Voiced_detection_patterns](https://user-images.githubusercontent.com/61736138/77792034-f0ac7d80-7067-11ea-85c9-0dfac27c8484.png)


      - Use el detector de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos detectores.
		
		En el siguiente gráfico se puede observar en primer lugar, el resultado obtenido en el archivo f0 con nuestro detector de pitch y, en segundo lugar, el que nos proporciona el própio Wavesurfer. A pesar de ser archivos muy parecidos, el nuestro presenta puntualmente unos picos no deseados, ya que el valor de la frecuencia de pitch tendría que ser constante y, en este aspecto es mejor el del Wavesurfer. A pesar de esto, los tramos sonoros y sordos se detectan a la perfección.
		
		![f0_pitchContour](https://user-images.githubusercontent.com/61736138/77792594-e76fe080-7068-11ea-8358-be2e36fce444.png)

  
  * Optimice los parámetros de su sistema de detección de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..
	
	Después de modificar los límites de los distintos parámetros del detector de pitch (cocientes de correlación y poténica), hemos dado con los valores que nos proporcionan un 90.67% de total en el evaluador de pitch. Dónde obtenemos más error es al detectar tramas sonoras como sordas (8.42%). Aunque también comentemos cierto error detectando tramas sordas como sonoras, este se reduce a casi la mitad que el anterior (4.67%).
	
	![TOTAL](https://user-images.githubusercontent.com/61736138/77794195-ba70fd00-706b-11ea-9c15-08d0bb58a7eb.png)

   * Inserte una gráfica en la que se vea con claridad el resultado de su detector de pitch junto al del
     detector de Wavesurfer. Aunque puede usarse Wavesurfer para obtener la representación, se valorará
	 el uso de alternativas de mayor calidad (particularmente Python).
   

Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  detector a los argumentos de la línea de comandos.
    
  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.
    
    En la siguiente captura se puede ver el mensaje de ayuda del programa una vez modificado:
    
    ![help_param](https://user-images.githubusercontent.com/61736138/77794650-88ac6600-706c-11ea-8f95-16889ea733f8.png)
    
    Para comprovar su correcto funcionamiento, vamos a evaluar el resultado obtenido en un mismo archivo de audio (rl002.wav) en el caso de introducir parámentro y en el caso que no. 
    Si no introducimos ningun parámetro al generar el archivo .f0; es decir, el programa usa los establecidos por defecto, obtenemos una puntuación del 89.23%.
    
    ![param_normals](https://user-images.githubusercontent.com/61736138/77794660-8ba75680-706c-11ea-8b2e-8df9fa64be4d.png)
    
    Por otro lado, si le indicamos el valor que queremos que tomen los parámetros, al no ser los ideales, el total obtenido se reduce considerablemente. La siguiente captura muestra un caso en el que se ha establecido el límite de poténcia a superar en -40dB y los límites de los coeficientes r[1]/r[0] y r[lag]/r[0] en 0.7 y 0.2 respectivamente; obteniendo así un 77.88%.
    
    ![param_modificats](https://user-images.githubusercontent.com/61736138/77794665-8e09b080-706c-11ea-96f4-634ad89ed9af.png)


- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de detección
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, *center clipping*, etc.
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el detector, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso](https://atenea.upc.edu/pluginfile.php/2908770/mod_resource/content/3/2b_PS Techniques.pdf)
  y en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al detector. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.
   

Evaluación *ciega* del detector
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
detector con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
