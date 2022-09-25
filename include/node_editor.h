#pragma once

namespace example
{
void NodeEditorInitialize();
void NodeEditorShow();
float* NodeEditorAudioCallback();
void NodeEditorAudioLoadInput(float* input_stream);
void NodeEditorShutdown();
} // namespace example
