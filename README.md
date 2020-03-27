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

- Una vez completados los puntos anteriores, dispondrá de una primera versión del detector de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la detección de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
		(r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
		autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).

		Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.

	    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
		en esta práctica es de 15 ms.

      - Use el detector de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos detectores.
  
  * Optimice los parámetros de su sistema de detección de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..

   * Inserte una gráfica en la que se vea con claridad el resultado de su detector de pitch junto al del
     detector de Wavesurfer. Aunque puede usarse Wavesurfer para obtener la representación, se valorará
	 el uso de alternativas de mayor calidad (particularmente Python).
   

Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  detector a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del detector. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la detección de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.

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
