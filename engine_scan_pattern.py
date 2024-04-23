#from vortex.driver.alazar import Board, Channel
#from vortex.acquire import AlazarConfig, AlazarAcquisition, InternalClock, ExternalClock, Input, AuxIOTriggerOut
#from vortex.process import CUDAProcessor, CUDAProcessorConfig
#from vortex.io import DAQmxIO, DAQmxConfig, AnalogVoltageOutput, AnalogVoltageInput, DigitalOutput
#from vortex.engine import Block, source, acquire_alazar_clock, find_rising_edges, compute_resampling, dispersion_phasor, SampleStrobe, SegmentStrobe, VolumeStrobe, Polarity

from vortex import Range, get_console_logger as gcl
from vortex.engine import EngineConfig, Engine, StackDeviceTensorEndpointInt8 as StackDeviceTensorEndpoint, AscanStreamEndpoint, CounterStreamEndpoint, GalvoTargetStreamEndpoint, GalvoActualStreamEndpoint, MarkerLogStorage, NullEndpoint
from vortex.format import FormatPlanner, FormatPlannerConfig, StackFormatExecutorConfig, StackFormatExecutor, SimpleSlice
from vortex.marker import Flags, VolumeBoundary, ScanBoundary
from vortex.scan import RasterScanConfig, RadialScanConfig, FreeformScanConfig, FreeformScan, SpiralScanConfig, SpiralScan, limits
from vortex.storage import SimpleStreamInt8, SimpleStreamUInt64, SimpleStreamFloat64, SimpleStreamConfig, SimpleStreamHeader, MarkerLog, MarkerLogConfig

#from vortex_tools.ui.backend import NumpyImageWidget
#from vortex_tools.ui.display import RasterEnFaceWidget, CrossSectionImageWidget

RASTER_FLAGS = Flags(0x1)
AIMING_FLAGS = Flags(0x2)

scan_dimension=3
bidirectional=False
ascans_per_bscan=125
bscans_per_volume=10
galvo_delay=190e-6

aiming_count = 10

clock_samples_per_second=int(1000e6)
blocks_to_acquire=0
ascans_per_block=512
samples_per_ascan=int(1376*2.25)
trigger_delay_seconds=0

blocks_to_allocate=128
preload_count=32

#swept_source=source.Axsun100k
#internal_clock=True
#clock_channel=Channel.B
#input_channel=Channel.A

process_slots=2
dispersion=(-3e-6, 0)

log_level=1




rsc = RasterScanConfig()
rsc.bscan_extent = Range(-scan_dimension, scan_dimension)
rsc.volume_extent = Range(-scan_dimension, scan_dimension)
rsc.bscans_per_volume = bscans_per_volume
rsc.ascans_per_bscan = ascans_per_bscan

rsc.bidirectional_segments = bidirectional
rsc.bidirectional_volumes = bidirectional
rsc.limits = [limits.ScannerMax_Saturn_5B]*2
#rsc.samples_per_second = self._cfg.swept_source.triggers_per_second
rsc.flags = RASTER_FLAGS

scan.initialize(rsc)

_, ax = plot_annotated_waveforms_space(scan.scan_buffer(), scan.scan_markers(), inactive_marker=None, scan_line='w-')
ax.set_title('Raster')


import numpy as np

data = scan.scan_buffer()

#Write the data points of the scan pattern into a txt file
d = open('EnginePattern.txt', 'w')
print(*data, file = d)

np.save('EnginePattern.npy', data)



len(data)
data[0]
datapoints = np.load('EnginePattern.npy')
lin_length = len(datapoints)




import struct
import numpy as np

SCALE_FACTOR = 15 

#Original
def ang_to_pos(ang):
    if abs(ang) >= SCALE_FACTOR:
        raise Exception("Magnitude of Angle Exceeds Scale Factor") 
    pos  = (ang  / SCALE_FACTOR) * 32768 + 32768
    pos = round(pos)
    return pos

def pos_to_ang(pos):
    if pos > 65535 or pos < 0:
        raise Exception("Value of Position Exceeds 2^16 - 1") 
    ang = ((pos - 32768) / 32768) * SCALE_FACTOR
    return ang

f = open('EnginePattern_hexadecimal.txt', 'w')

for i in range(0, len(datapoints)):
    pointX = datapoints[i][0]
    hexaX = hex(ang_to_pos(pointX))
    hexaX = hexaX[2:]
    pointY = datapoints[i][1]
    hexaY = hex(ang_to_pos(pointY))
    hexaY = hexaY[2:]
    print(hexaX + hexaY, file = f)
    print(i)




len(datapoints)
lin_length

f = open('EnginePattern_hexadecimal_Downsample.txt', 'w')
for j in range(0, 4):
    for i in range(0, len(datapoints)):
        pointX = datapoints[i][0]
        hexaX = hex(ang_to_pos(pointX))
        hexaX = hexaX[2:]
        pointY = datapoints[i][1]
        hexaY = hex(ang_to_pos(pointY))
        hexaY = hexaY[2:]
        if(i % 4 == 3):
            print(hexaX + hexaY, file = f)
            print(i)
