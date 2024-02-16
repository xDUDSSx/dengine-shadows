#pragma once

#include "dengine/platform/CommonGL.h"
#include "GLFW/glfw3.h"

namespace Dg
{
/// The "Tools" namespace borrowed from PGR2 c project
namespace Profiling
{
//-----------------------------------------------------------------------------
// Name: BufferRelease
// Desc:
//-----------------------------------------------------------------------------
template <int target, int query>
class BufferRelease {
  public:
	BufferRelease() {
		glGetIntegerv(query, &buffer);
		if (buffer > 0)
			glBindBuffer(target, 0);
	}
	~BufferRelease() {
		if (buffer > 0)
			glBindBuffer(target, buffer);
	}
  private:
	GLint buffer;
};
typedef BufferRelease<GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING>                             VertexBufferRelease;
typedef BufferRelease<GL_ELEMENT_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER_BINDING>             IndexBufferRelease;
typedef BufferRelease<GL_QUERY_BUFFER, GL_QUERY_BUFFER_BINDING>                             QueryBufferRelease;
typedef BufferRelease<GL_SHADER_STORAGE_BUFFER, GL_SHADER_STORAGE_BUFFER_BINDING>           ShaderStorageBufferRelease;
typedef BufferRelease<GL_TRANSFORM_FEEDBACK_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER_BINDING>   TransformFeedbackBufferRelease;
typedef BufferRelease<GL_UNIFORM_BUFFER, GL_UNIFORM_BUFFER_BINDING>                         UniformBufferRelease;


//-----------------------------------------------------------------------------
// Name: GetCPUTime()
// Desc:
//-----------------------------------------------------------------------------
inline double GetCPUTime(bool micro = true) {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//	return glfwGetTime() * (micro ? 1000000 : 1000000000);
}


//-----------------------------------------------------------------------------
// Name: GetGPUTime()
// Desc:
//-----------------------------------------------------------------------------
inline double GetGPUTime(bool micro = true) {
	GLint64 time = 0;
	glGetInteger64v(GL_TIMESTAMP, &time);
	return micro ? time * 0.001 : time;
}


// Very simple GPU timer (timer cannot be nested with other timer or GL_TIME_ELAPSED query
class GPUTimer {
  public:
	GPUTimer() : queryStart(0), queryStop(0), time(0), timeTotal(0), counter(0) {}
	~GPUTimer() {
		if (queryStart)
			glDeleteQueries(1, &queryStart);
		if (queryStop)
			glDeleteQueries(1, &queryStop);
	}

	void start(bool sync = false) {
		if (queryStart == 0) {
			glCreateQueries(GL_TIMESTAMP, 1, &queryStart);
			glCreateQueries(GL_TIMESTAMP, 1, &queryStop);
		}
		time = 0;
		if (sync) glFinish();
		glQueryCounter(queryStart, GL_TIMESTAMP);
	}

	void stop(bool sync = false) const {
		if (sync) glFinish();
		if (time == 0) {
			glQueryCounter(queryStop, GL_TIMESTAMP);
		}
	}

	unsigned int get(bool micro = true) {   // micro - show in microsec, otherwise in nanosec
		if ((time == 0) && (queryStart > 0) && (queryStop > 0)) {
			// Unbind query buffer if bound
			QueryBufferRelease queryBufferRelease;

			GLuint64 start_time = 0, stop_time = 0;
			glGetQueryObjectui64v(queryStart, GL_QUERY_RESULT, &start_time);
			glGetQueryObjectui64v(queryStop, GL_QUERY_RESULT, &stop_time);
			time = stop_time - start_time;
			timeTotal += time;
			counter++;
		}
		return micro ? static_cast<unsigned int>(time * 0.001f) : static_cast<unsigned int>(time);
	}

	unsigned int getAverage(bool micro = true) {   // micro - show in microsec, otherwise in nanosec
		get(micro);
		float const result = static_cast<float>(timeTotal) / counter + 0.5f;
		return static_cast<unsigned int>(micro ? result * 0.001f : result);
	}

	unsigned int getCounter() const {
		return counter;
	}

	void reset() {
		timeTotal = 0;
		counter   = 0;
	}

	GLuint getId(bool start) const {
		return start ? queryStart : queryStop;
	}

  private:
	GLuint   queryStart, queryStop;
	GLuint64 time;
	GLuint64 timeTotal;
	GLuint   counter;
};

// Very simple GPU timer (timer cannot be nested with other timer or GL_TIME_ELAPSED query
class GPURangeTimer {
  public:
	GPURangeTimer() : query(0), time(0), timeTotal(0), counter(0) {}
	~GPURangeTimer() {
		if (query)
			glDeleteQueries(1, &query);
	}

	void start(bool sync = false) {
		if (query == 0) glCreateQueries(GL_TIME_ELAPSED, 1, &query);
		time = 0;
		if (sync) glFinish();
		glBeginQuery(GL_TIME_ELAPSED, query);
	}

	void stop(bool sync = false) const {
		if (sync) glFinish();
		if ((time == 0) && (query > 0)) {
			glEndQuery(GL_TIME_ELAPSED);
		}
	}

	unsigned int get(bool micro = true) {   // micro - show in microsec, otherwise in nanosec
		// Unbind query buffer if bound
		QueryBufferRelease queryBufferRelease;

		if ((time == 0) && (query > 0)) {
			glGetQueryObjectui64v(query, GL_QUERY_RESULT, &time);
			timeTotal += time;
			counter++;
		}
		return micro ? static_cast<unsigned int>(time * 0.001f) : static_cast<unsigned int>(time);
	}

	unsigned int getAverage(bool micro = true) {   // micro - show in microsec, otherwise in nanosec
		get(micro);
		float const result = static_cast<float>(timeTotal) / counter + 0.5f;
		return static_cast<unsigned int>(micro ? result * 0.001f : result);
	}

	unsigned int getCounter() const {
		return counter;
	}

	void reset() {
		time      = 0;
		timeTotal = 0;
		counter   = 0;
	}

	GLuint getId() const {
		return query;
	}

  private:
	GLuint   query;
	GLuint64 time;
	GLuint64 timeTotal;
	GLuint   counter;
};

// Very simple CPU timer
class CPUTimer {
  public:
	CPUTimer() : timeStart(0), timeStop(0), time(0), timeTotal(0), counter(0) {}

	void start() {
		time = 0;
		timeStart = GetCPUTime(false);
	}

	void stop() {
		if (time == 0) {
			timeStop = GetCPUTime(false);
		}
	}

	unsigned int get(bool micro = true) {
		if (time == 0) {
			time = timeStop - timeStart;
			timeTotal += time;
			counter++;
		}
		return micro ? static_cast<unsigned int>(time * 0.001f) : static_cast<unsigned int>(time);
	}

	unsigned int getAverage(bool micro = true) {
		get(micro);
		float const result = static_cast<float>(timeTotal) / counter + 0.5f;
		return static_cast<unsigned int>(micro ? result * 0.001f : result);
	}

	unsigned int getCounter() const {
		return counter;
	}

	void reset() {
		time      = 0;
		timeTotal = 0;
		counter   = 0;
	}

  private:
	double             timeStart, timeStop;
	double             time;
	unsigned long long timeTotal;
	unsigned int       counter;
};
};

} // namespace Dg
