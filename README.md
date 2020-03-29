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
	
	Después de modificar los límites de los distintos parámetros del detector de pitch (cocientes de correlación y poténica), hemos dado con los valores que nos proporcionan un 91.06% de total en el evaluador de pitch. Dónde obtenemos más error es al detectar tramas sonoras como sordas (8.42%). Aunque también comentemos cierto error detectando tramas sordas como sonoras, este se reduce a casi la mitad que el anterior (4.67%).
	
	![TOTAL](https://user-images.githubusercontent.com/61736138/77848459-0d11fc80-71c5-11ea-8727-47d3ffe9d0ca.png)

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

  Al detector se le han incluido una técnica de preprocesado: *center clipping* y una técnica de postprocesado: el filtro de mediana.
  El *center clipping* utiliza un umbral situado en un 1% respecto la valor màximo de la señal. El porcentaje elegido ha sido muy bajo porque usando este procedimiento, no conseguimos mejorar el mejor resultado obtenido sin usar procedimientos adicionales. Cuanto mayor sea el valor del umbral, peores resultados da. Aún así, con el valor de umbral elegido conseguimos aproximarnos mucho al valor que obtuvimos, aunque modificando el valor de los parámetros elegidos, ya que este método disminuye la potencia de todas las tramas (con el portentaje introducido, esta variación es muy leve). Se ha decidido mantener la implementación de este método para demostrar que el trabajo ha sido realizado, ya que su influencia con el valor introducido es mínima.
   
   
   ``` cpp
  float maxvalue=0.0F;
  for(unsigned int k = 0; k< x.size(); k++){
    if(x[k]> maxvalue) maxvalue = x[k];
  }
  float llindar_CC=0.01*maxvalue;
  for(unsigned int k = 0; k < x.size(); k++){
    if (abs(x[k]) < llindar_CC) x[k]=0;
    else if (x[k]> llindar_CC) x[k]-=llindar_CC;
    else if (x[k] < -llindar_CC) x[k]+=llindar_CC;
  }
  ```
  
  El código realizado se encarga de encontrar la valor máximo y posteriormente modificar el valor de las tramas, poniendo aquellas que se encuentran entre el umbral con signo positivo y el mismo umbral con signo negativo a 0, restando el valor del umbral a aquellas tramas que estén por encima de éste y sumando el valor del umbral a aquellas tramas que estén por debajo del valor del umbral con signo negativo. El efecto se puede entender más facilmente con esta imagen:
  
 	![center_clipping](https://user-images.githubusercontent.com/61736138/77829256-2e211180-7121-11ea-90c0-e0fa6f8982f6.jpg)

	El filtro de mediana utilitza una ventana longitud de 3, ya que es el que da unos resultados mejores a los que se tenían antes de realizar un postprocesado, ya que los filtros de tamaño 5 y 7 lo empeoran.
	
	``` cpp
	int medianLength = 3;
	  for (unsigned int i = (medianLength-1)/2; i < f0.size() - (medianLength-1)/2; ++i)
	  {
	    float window[medianLength];
	    for (int j = 0; j < medianLength; ++j)
		window[j] = f0[i - (medianLength-1)/2 + j];
	    for (int j = 0; j < medianLength - (medianLength-1)/2; ++j)
	    {
		int min = j;
		for (int k = j + 1; k < medianLength; ++k)
		  if (window[k] < window[min])
		      min = k;
		const float temp = window[j];
		window[j] = window[min];
		window[min] = temp;
	    }
	    f0[i] = window[(medianLength-1)/2];
	  }
  
	```
	El código realizado recorre todos los elementos de la señal, desde el segundo hasta el penúltimo (para el caso de longitud del filtro 3). Para cada trama sobre la que se pasa el filtro, se coje su anterior, su posterior y esa misma para ordenarlas de menor a mayor en un buffer. Entonces se le da a la trama el valor que tenga la trama central de ese buffer. El resultado con y sin el filtrado se observa en la siguiente imagen:

	![Post_procesado](https://user-images.githubusercontent.com/61736138/77829383-fc5c7a80-7121-11ea-9ced-b2d59b9f0a89.png)
	
	Estos dos procedimientos dan un total de 91.06% como máximo, ligeramente inferior al obtenido sin usarlos debido al beneficio negativo del primero de ellos. Los valores usados para obtener este resultado son los que se han introducen por defecto al ejecutar el código sin parámetros adicionales.

  También se ha intentado usar un parámetro adicional como condición de decisión de una trama sonora o sorda. Este parámetro define un nivel de potencia a partir del cual una trama siempre va a ser sonora. Finalmente esto ha sido eliminado ya que el valor de esta potencia no modificaba el resultado o lo empeoraba.
s alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.
