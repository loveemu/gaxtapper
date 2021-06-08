Gaxtapper
=========
[![Travis Build Status](https://travis-ci.com/loveemu/gaxtapper.svg?branch=main)](https://travis-ci.com/loveemu/gaxtapper) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/8uj0j4si32p6msp8/branch/main?svg=true)](https://ci.appveyor.com/project/loveemu/gaxtapper/branch/main)

**WARNING**: This is still under development. Please use it for testing and evaluation purposes only.

[Gaxtapper](https://github.com/loveemu/gaxtapper) is an automated GSF ripper for GAX Sound Engine. Gaxtapper is able to create [GSF format](https://www.caitsith2.com/gsf/) soundtracks from Game Boy Advance ROMs that use [Shin'en's GAX Sound Engine](https://www.shinen.com/music/music.php3?gax). It is as easy to use as its adorable sister [Saptapper](https://github.com/loveemu/saptapper).

GSF files can be played by music players such as [foobar2000](https://www.foobar2000.org/) (using [foo_input_gsf](https://www.foobar2000.org/components/view/foo_input_gsf)).

## Installation

The stable releases are available on the [Releases](https://github.com/loveemu/gaxtapper/releases) page (**no, not yet**). The latest Windows executable is available on [Appveyor](https://ci.appveyor.com/project/loveemu/gaxtapper/branch/main).

To build Gaxtapper by yourself, you will need the following tools:

* [Visual Studio](https://visualstudio.microsoft.com/ja/vs/community/) 2017 or later (Windows)
* [CMake](https://cmake.org/) and a compiler that supports C++17 or later (other platforms)
* [devkitARM](https://devkitpro.org/) (optional, to assemble driver code for GBA) 

## How to use

Gaxtapper is a command-line tool. To use this, you usually need to open a terminal such as Command Prompt or [Windows Terminal](https://www.microsoft.com/p/windows-terminal/9n0dx20hk701). If you are unfamiliar with it, you may want to know the basics of the command line in advance.

### Create a soundtrack

Use `gaxtapper extract` to create the gsflib/minigsf files from the ROM file. If the output directory path is omitted, the file will be created in the current directory.

**Pro Tip**: The entry point address and work RAM address used by the driver can be changed with optional arguments. Check the built-in help for details.

```cmd
gaxtapper extract -d output_directory "Maya The Bee.gba"
```

#### Optimizing, timing and tagging

**IMPORTANT**: Gaxtapper does not optimize the ROM. We highly recommend using [gsfopt](https://github.com/loveemu/gsfopt) to remove unreferenced code and graphics. The following command is typically used for optimization.

```cmd
gsfopt -l *.minigsf
```

If you prefer, you can set the timer for each song automatically (the accuracy of the result depends on the case).

```cmd
gsfopt -t -T *.minigsf
```

[PSFPoint](https://github.com/loveemu/psfpoint) can also be used to perform all tagging in a command-line fashion. 

```cmd
psfpoint -game="Maya The Bee" *.minigsf
```

### Check GAX compatibility / List included songs

Use `gaxtapper inspect` to check whether the ROM is compatible with the Gaxtapper without creating a file, or to display the list of songs in the ROM. Multiple ROM files can be specified.

```cmd
gaxtapper inspect "Maya the Bee.gba" "Shark Tale.gba"
```

Use `gaxtapper inspect -S` for a simple one-line display of the GAX driver version.

```
gaxtapper inspect -S *.gba
```

### Customize playback parameters

Not available yet. Since GAX can change the mixing rate and volume for each song, we would like to be able to customize those settings in Gaxtapper.

### Need help?

You can check the command syntax as follows.

```cmd
gaxtapper --help
```

## Minigsf program block format

Advanced users can change the playback settings by editing the minigsf program block with an external tool.

The format is as follows. Note that this is the current implementation version, and does not necessarily correspond to past or future formats.

|Name        |Offset |Size |Description                                                                                                                          |
|------------|-------|-----|-------------------------------------------------------------------------------------------------------------------------------------|
|music       |0      |4    |The address of the song header                                                                                                       |
|fx          |4      |4    |The address of the sound FX instrument header (can be 0 if FX is not used)                                                           |
|fxid        |8      |2    |**TBA**: Set to 0xffff if FX is not used. This property is not yet supported, so always set it to 0xffff                             |
|flags       |0xA    |2    |Flags for playback configuration. See below for details. Usually set to 0                                                            |
|mixing_rate |0xC    |2    |Mixing rate of music and FX in hertz (use 5735, 9079, 10513, 11469, 13380, 15769, 18158, 21025, 26760, 31537, 36316, 40138 or 42049) |
|volume      |0xE    |2    |Volume. Set to 0xffff when there is no need to specify. The standard volume is 0x100                                                 |

The details of the flags are not yet clear, but at least the following flags are available:

|Name            |Value |Description                                                                               |
|----------------|------|------------------------------------------------------------------------------------------|
|GAX_JINGLE      |8     |Play module as jingle (no loop)                                                           |
|GAX_NO_JINGLE   |0x10  |Disable jingle playback by gax2_jingle function. This will reduce resource usage          |
|GAX_FX_REVERB   |?     |Unknown                                                                                   |
|GAX_HALFRATE_FX |?     |Set the FX mixing rate to half of the music and use the same one DMA for playback (maybe) |

## Compatibility, limitations and known bugs

GAX is a little less self-contained than M4A (Nintendo's standard sound driver) and can change things programmatically, so sometimes Gaxtapper may not work as expected. If you think you have found a problem, please visit the [Issues](https://github.com/loveemu/gaxtapper/issues) page.

### Supported / Unsupported games

GAX 1.x / 2.x is not supported at this time (only detection supported). 

Most games developed by Vicarious Visions are not supported. It's because they transfer the division routine into internal WRAM for speed, but Gaxtapper cannot handle it specially ([#13](https://github.com/loveemu/gaxtapper/issues/13)).

Gaxtapper will output an invalid GSF for Jazz Jackrabbit. Fortunately, there is a workaround for this ([#17](https://github.com/loveemu/gaxtapper/issues/17)).

### Limitations

* One-shot jingles may loop infinitely, as there is no way provided to change the playback method at the moment ([#5](https://github.com/loveemu/gaxtapper/issues/5))
* Sound effects cannot be extracted (may include music composed of streamed audio) ([#24](https://github.com/loveemu/gaxtapper/issues/12))
* Gaxtapper cannot reproduce the fade-in of music when the volume is dynamically changed outside the GAX ([#14](https://github.com/loveemu/gaxtapper/issues/14))

## Technical explanation

[Research Note: Shin'en GAX Sound Engine V3 (GBA)](https://gist.github.com/loveemu/9b3063ffd9a76cb18e379324e43f3251)

Gaxtapper scans the entire ROM using function signatures and simple heuristics for the necessary code and music data. Then, it will insert the relocatable driver code block into the ROM and dynamically link the address found in the previous step. This is the same as Saptapper.

As for where to insert the driver code block. Saptapper will somehow try to find a free space in ROM, while Gaxtapper will simply use the original entry point. In practice, this method is rarely a problem. (In the initial development I always used 0x80000c0, which sometimes broke the system call function.)

Also, in contrast to M4A, the GSF driver has to pass the work area to GAX. In addition, GAX maintains a pointer to that area at a static address in IWRAM or EWRAM. If the two conflict, the memory will be corrupted ([#7](https://github.com/loveemu/gaxtapper/issues/7)). To avoid conflicts, Gaxtapper scans the address of the internal pointer held by GAX and then dynamically determines the address of the work area on IWRAM. (EWRAM can also be used, however there are rare cases where slow RAM access makes music playback slower.)

## License

See [LICENSE.txt](https://github.com/loveemu/gaxtapper/blob/main/LICENSE.txt).

## Special thanks to

* Shin'en and Bernhard Wodok: For creating GAX and great music
* Caitsith2: For designing GSF format and tools including Saptapper
* Incineroar: For sharing research notes and IDB for Crash Bandicoot: The Huge Adventure. This was the starting point for me to start looking at GAX
* Anterag and LAC: For a huge amount of testing and feedback
