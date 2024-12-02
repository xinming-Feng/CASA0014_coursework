# CASA0014_coursework
## Prototypes and ideas
Temperature and humidity are two important indicators in daily life. The change of temperature and humidity can lead to a change in people's body comfort. However, in daily life, people can intuitively see the changes in temperature and humidity, but can't observe the indoor or outdoor body comfort. The comfortable temperature (or humidity) plus the uncomfortable humidity (or temperature) will also lead to bad body feeling.</br>

Therefore, I hope to have an index that can combine temperature and humidity, and obtain an index through certain calculations, which more intuitively reflects the direct effect of temperature and humidity on human comfort. And convert the values into color information, to Chrono Lumina, so that people can more quickly and directly see the body comfort.</br>

The index I used is heat index(HI), which can combine temperature and humidity in a hot environment.</br>

## Device Introduction 
### Sensor
DHT22
### Functionality
1.When the heat index goes from low to high, the color of the light changes from Blue -&gt; Green -&gt; Yellow -&gt; Red. Thus, the body comfort can be intuitively seen.</br>
2.At the same time, for some people with color cognitive impairment (such as color weakness), the number and brightness of the lights can be used to display the value of heat index (for example, when the value of heat index is low, the lights will be less, and when the value of heat index is high, the lights will be more).</br>

## Structure

<strong>first_versin</stong> :The original version did not use heat index, only mixed temperature and humidity, tiled to RGB three channels, and did not have obvious color development.</br>
<strong>second_versin</stong> :Some temperature conditions were set to make the RGB color more obvious, but the actual values were not meaningful.</br>
<strong>third_versin</stong> :The heat index is used to control the color of the lamp, but only for a single light.</br>
<strong>final_version_multi</stong> :Changes in the value of the heat index are mapped to the number and brightness of lights, but have not been tested.




