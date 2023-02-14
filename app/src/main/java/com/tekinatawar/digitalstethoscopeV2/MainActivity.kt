package com.tekinatawar.digitalstethoscopeV2

import android.content.pm.ActivityInfo
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels

import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.VolumeMute
import androidx.compose.material.icons.filled.VolumeUp
import androidx.compose.runtime.Composable
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.rotate
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.lifecycle.ViewModel
import com.tekinatawar.digitalstethoscopeV2.ui.theme.DigitalStethoscopeTheme

class MainActivity : ComponentActivity() {

    private val synthesizerViewModel: WavetableSynthesizerVeiwModel by viewModels()
    private val synthesizer = NativeWavetableSynthesizer()


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE

        lifecycle.addObserver(synthesizer)

        synthesizerViewModel.wavetableSynthesizer = synthesizer
        setContent {
            DigitalStethoscopeTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colors.background
                ) {
                    digitalStethoscopeApp(Modifier, synthesizerViewModel)
                }
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()

        lifecycle.removeObserver(synthesizer)
    }

    override fun onResume() {
        super.onResume()
        synthesizerViewModel.applyParameters()
    }

}

@Composable
fun digitalStethoscopeApp(
    modifier: Modifier,
    synthesizerViewModel: WavetableSynthesizerVeiwModel
) {
    Column(
        modifier = modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Top,
    ) {
        dSSelectionPanel(modifier, synthesizerViewModel)
        ControlsPanel(modifier, synthesizerViewModel)
    }
}

@Composable
fun dSSelectionPanel(
    modifier: Modifier,
    synthesizerViewModel: WavetableSynthesizerVeiwModel
) {
    Row(
        modifier = modifier
            .fillMaxWidth()
            .fillMaxHeight(0.5f),
        horizontalArrangement = Arrangement.SpaceEvenly,
        verticalAlignment = Alignment.CenterVertically
    ) {

        Column(
            modifier = modifier.fillMaxSize(),
            verticalArrangement = Arrangement.SpaceEvenly,
            horizontalAlignment = Alignment.CenterHorizontally
        ){
            Text(stringResource(R.string.wavetable))
            WavetableSelectionButtons(modifier, synthesizerViewModel)
        }

    }
}

@Composable
fun WavetableSelectionButtons (
    modifier: Modifier,
    synthesizerViewModel: WavetableSynthesizerVeiwModel
) {
    Row(
        modifier = modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.SpaceEvenly
    ){
        for (wavetable in Wavetable.values()) {
            WavetableButton(
                modifier = modifier,
                onClick  = {synthesizerViewModel.setWavetable(wavetable)},
                label = stringResource(wavetable.toResourceString())
            )
        }
    }
}

@Composable
fun WavetableButton(
    modifier: Modifier,
    onClick: () -> Unit,
    label: String
) {
    Button(modifier = modifier, onClick = onClick) {
        Text(label)
    }
}

@Composable
fun ControlsPanel(
    modifier: Modifier,
    synthesizerViewModel: WavetableSynthesizerVeiwModel
){
    Row (
        modifier = Modifier
            .fillMaxWidth()
            .fillMaxHeight(),
        horizontalArrangement = Arrangement.Center,
        verticalAlignment = Alignment.CenterVertically
    ){
        Column(
            modifier = modifier
                .fillMaxHeight()
                .fillMaxWidth(0.7f),
            horizontalAlignment = Alignment.CenterHorizontally
        ){
            PitchControl(modifier, synthesizerViewModel)
            PlayControl(modifier, synthesizerViewModel)
        }
        Column(
            modifier = modifier
                .fillMaxSize(),
            verticalArrangement = Arrangement.SpaceEvenly,
            horizontalAlignment = Alignment.CenterHorizontally,
        ){
            VolumeControl(modifier, synthesizerViewModel)
        }
    }
}

@Composable
fun PitchControl(
    modifier: Modifier,
    synthesizerViewModel: WavetableSynthesizerVeiwModel
){
//    val frequency = rememberSaveable { mutableStateOf(300F)}
    val frequency = synthesizerViewModel.frequency.observeAsState()
    PitchControlContent(
        modifier,
        pitchControlLabel = stringResource(R.string.frequency),
        value = synthesizerViewModel.sliderPositionFromFrequencyInHz(frequency.value!!) ,
        onValueChange = {
            synthesizerViewModel.setFrequencySliderPosition(it)
        },
        valueRange = 0f..1f,
        frequencyValueLabel = stringResource(R.string.frequency_value, frequency.value!!)
    )
}

@Composable
fun PitchControlContent(
    modifier: Modifier,
    pitchControlLabel: String,
    value: Float,
    onValueChange: (Float) -> Unit,
    valueRange: ClosedFloatingPointRange<Float>,
    frequencyValueLabel : String
){
    Text(pitchControlLabel)
    Slider(modifier = modifier, value = value , onValueChange = onValueChange, valueRange = valueRange)
    Text(frequencyValueLabel)
}

@Composable
fun PlayControl(
    modifier: Modifier,
    synthesizerViewModel: WavetableSynthesizerVeiwModel
){
    val playButtonLabel = synthesizerViewModel.playButtonLabel.observeAsState()

    Button(modifier = modifier,
    onClick = {synthesizerViewModel.playClicked()}) {
        Text(stringResource(playButtonLabel.value!!))
    }
}

@Composable
fun VolumeControl(
    modifier: Modifier,
    synthesizerViewModel: WavetableSynthesizerVeiwModel
){
//    val volume = rememberSaveable{(mutableStateOf(-10F) ) }
    val volume = synthesizerViewModel.volume.observeAsState()

    VolumeControlContent(modifier = modifier,
    value = volume.value!!,
    onValueChange = {
//        volume.value = it
        synthesizerViewModel.setVolume(it)
    },
    volumeRange = synthesizerViewModel.volumeRange
    )
}

@Composable
fun VolumeControlContent(
    modifier: Modifier,
    value: Float,
    onValueChange: (Float) -> Unit,
    volumeRange: ClosedFloatingPointRange<Float>
) {
    val screenHeight = LocalConfiguration.current.screenHeightDp
    val sliderHeight = screenHeight / 4

    Icon(imageVector = Icons.Filled.VolumeUp, contentDescription = null )
    Slider(
        value = value,
        onValueChange = onValueChange,
        modifier = modifier
            .width(sliderHeight.dp)
            .rotate(270f), valueRange = volumeRange)
    Icon(imageVector = Icons.Filled.VolumeMute, contentDescription = null )

}