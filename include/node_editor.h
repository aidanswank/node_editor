#pragma once

namespace example
{
void NodeEditorInitialize();
void NodeEditorShow();
float* NodeEditorAudioCallback();
void NodeEditorAudioLoadInput(float* input_stream, int chan_count);
void NodeEditorShutdown();
} // namespace example
