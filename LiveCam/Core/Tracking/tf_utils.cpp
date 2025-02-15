// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2019 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "tf_utils.h"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace tf_utils {

	namespace {
		static void DeallocateBuffer(void* data, size_t) {
			std::free(data);
		}

		static TF_Buffer* ReadBufferFromFile(const char* file) {
			std::ifstream f(file, std::ios::binary);
			if (f.fail() || !f.is_open()) {
				return nullptr;
			}

			f.seekg(0, std::ios::end);
			const auto fsize = f.tellg();
			f.seekg(0, std::ios::beg);

			if (fsize < 1) {
				f.close();
				return nullptr;
			}

			char* data = static_cast<char*>(std::malloc(fsize));
			f.read(data, fsize);
			f.close();

			TF_Buffer* buf = TF_NewBuffer();
			buf->data = data;
			buf->length = fsize;
			buf->data_deallocator = DeallocateBuffer;

			return buf;
		}

	} // namespace tf_utils::

	TF_Graph* LoadGraph(const char* graphPath) {
		if (graphPath == nullptr) {
			return nullptr;
		}

		TF_Buffer* buffer = ReadBufferFromFile(graphPath);
		if (buffer == nullptr) {
			return nullptr;
		}

		TF_Graph* graph = TF_NewGraph();
		TF_Status* status = TF_NewStatus();
		TF_ImportGraphDefOptions* opts = TF_NewImportGraphDefOptions();

		TF_GraphImportGraphDef(graph, buffer, opts, status);
		TF_DeleteImportGraphDefOptions(opts);
		TF_DeleteBuffer(buffer);

		if (TF_GetCode(status) != TF_OK) {
			TF_DeleteGraph(graph);
			graph = nullptr;
		}

		TF_DeleteStatus(status);

		return graph;
	}

	void DeleteGraph(TF_Graph* graph) {
		TF_DeleteGraph(graph);
	}

	TF_Session* CreateSession(TF_Graph* graph) {
		TF_Status* status = TF_NewStatus();
		TF_SessionOptions* options = TF_NewSessionOptions();
		TF_Session* session = TF_NewSession(graph, options, status);
		TF_DeleteSessionOptions(options);

		if (TF_GetCode(status) != TF_OK) {
			DeleteSession(session);
			TF_DeleteStatus(status);
			return nullptr;
		}
		TF_DeleteStatus(status);

		return session;
	}

	void DeleteSession(TF_Session* session) {
		TF_Status* status = TF_NewStatus();
		TF_CloseSession(session, status);
		if (TF_GetCode(status) != TF_OK) {
			TF_CloseSession(session, status);
		}
		TF_DeleteSession(session, status);
		if (TF_GetCode(status) != TF_OK) {
			TF_DeleteSession(session, status);
		}
		TF_DeleteStatus(status);
	}

	TF_Code RunSession(TF_Session* session,
		const TF_Output* inputs, TF_Tensor* const* input_tensors, std::size_t ninputs,
		const TF_Output* outputs, TF_Tensor** output_tensors, std::size_t noutputs) {
		if (session == nullptr ||
			inputs == nullptr || input_tensors == nullptr ||
			outputs == nullptr || output_tensors == nullptr) {
			return TF_INVALID_ARGUMENT;
		}

		TF_Status* status = TF_NewStatus();
		TF_SessionRun(session,
			nullptr, // Run options.
			inputs, input_tensors, static_cast<int>(ninputs), // Input tensors, input tensor values, number of inputs.
			outputs, output_tensors, static_cast<int>(noutputs), // Output tensors, output tensor values, number of outputs.
			nullptr, 0, // Target operations, number of targets.
			nullptr, // Run metadata.
			status // Output status.
		);

		TF_Code code = TF_GetCode(status);
		TF_DeleteStatus(status);
		return code;
	}

	TF_Code RunSession(TF_Session* session,
		const std::vector<TF_Output>& inputs, const std::vector<TF_Tensor*>& input_tensors,
		const std::vector<TF_Output>& outputs, std::vector<TF_Tensor*>& output_tensors) {
		return RunSession(session,
			inputs.data(), input_tensors.data(), input_tensors.size(),
			outputs.data(), output_tensors.data(), output_tensors.size());
	}

	TF_Tensor* CreateTensor(TF_DataType data_type,
		const std::int64_t* dims, std::size_t num_dims,
		const void* data, std::size_t len) {
		if (dims == nullptr) {
			return nullptr;
		}

		TF_Tensor* tensor = TF_AllocateTensor(data_type, dims, static_cast<int>(num_dims), len);
		if (tensor == nullptr) {
			return nullptr;
		}

		void* tensor_data = TF_TensorData(tensor);
		if (tensor_data == nullptr) {
			TF_DeleteTensor(tensor);
			return nullptr;
		}

		if (data != nullptr) {
			std::memcpy(tensor_data, data, std::min(len, TF_TensorByteSize(tensor)));
		}

		return tensor;
	}

	TF_Tensor* CreateEmptyTensor(TF_DataType data_type, const std::int64_t* dims, std::size_t num_dims) {
		return CreateTensor(data_type, dims, num_dims, nullptr, 0);
	}

	TF_Tensor* CreateEmptyTensor(TF_DataType data_type, const std::vector<std::int64_t>& dims) {
		return CreateEmptyTensor(data_type, dims.data(), dims.size());
	}

	void DeleteTensor(TF_Tensor* tensor) {
		if (tensor != nullptr) {
			TF_DeleteTensor(tensor);
		}
	}

	void DeleteTensors(const std::vector<TF_Tensor*>& tensors) {
		for (auto t : tensors) {
			TF_DeleteTensor(t);
		}
	}

	void SetTensorsData(TF_Tensor* tensor, const void* data, std::size_t len) {
		void* tensor_data = TF_TensorData(tensor);
		if (tensor_data != nullptr) {
			std::memcpy(tensor_data, data, std::min(len, TF_TensorByteSize(tensor)));
		}
	}

	TF_SessionOptions* CreateSessionOptions(double gpu_memory_fraction) {
		// See https://github.com/Neargye/hello_tf_c_api/issues/21 for details.
		TF_Status* status = TF_NewStatus();
		TF_SessionOptions* options = TF_NewSessionOptions();

		// The following is an equivalent of setting this in Python:
		// config = tf.ConfigProto( allow_soft_placement = True )
		// config.gpu_options.allow_growth = True
		// config.gpu_options.per_process_gpu_memory_fraction = percentage

		// Create a byte-array for the serialized ProtoConfig, set the mandatory bytes (first three and last four)
		std::array<std::uint8_t, 15> config = { 0x32, 0xb, 0x9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x1, 0x38, 0x1 };

		// Convert the desired percentage into a byte-array.
		std::uint8_t* bytes = reinterpret_cast<std::uint8_t*>(&gpu_memory_fraction);

		// Put it to the config byte-array, from 3 to 10:
		for (std::size_t i = 0; i < sizeof(gpu_memory_fraction); ++i) {
			config[i + 3] = bytes[i];
		}

		TF_SetConfig(options, config.data(), config.size(), status);

		if (TF_GetCode(status) != TF_OK) {
			TF_DeleteStatus(status);
			return nullptr;
		}

		TF_DeleteStatus(status);
		return options;
	}

} // namespace tf_utils