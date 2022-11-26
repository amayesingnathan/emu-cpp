#include "glpch.h"
#include "Framebuffer.h"

#include "glad/glad.h"

#pragma warning(push)
#pragma warning(disable: 4267)

namespace Emu {

    namespace Utils {

        static GLenum TextureTarget(bool multisampled);
        static void CreateTextures(bool multisampled, uint* outID, int count);
        static void BindTexture(bool multisampled, uint id);

        static void AttachColourTexture(uint id, int samples, GLenum internalFormat, GLenum format, int width, int height, int index);
        static void AttachDepthTexture(uint id, int samples, GLenum format, GLenum attachmentType, int width, int height);
        static bool IsDepthFormat(FramebufferTextureFormat format);
    }

    Framebuffer::Framebuffer(const FramebufferSpec& spec)
        : mSpecification(spec)
    {
        for (auto spec : mSpecification.attachments.attachments)
        {
            if (!Utils::IsDepthFormat(spec))
                mColourAttachmentSpecs.emplace_back(spec);
            else
                mDepthAttachmentSpec = spec;
        }

        Invalidate();
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &mRendererID);
        glDeleteTextures(mColourAttachments.size(), mColourAttachments.data());
        glDeleteTextures(1, &mDepthAttachment);
    }

    void Framebuffer::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
        glViewport(0, 0, mSpecification.width, mSpecification.height);
    }

    void Framebuffer::unbind() 
    { 
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    }

    void Framebuffer::blit(int screenWidth, int screenHeight) 
    {
        glBlitNamedFramebuffer(mRendererID, 0, 0, 0, mSpecification.width, mSpecification.height, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    void Framebuffer::resize(usize width, usize height)
    {
    }

    void Framebuffer::clearAttachment(uint attachmentIndex, int value)
    {
    }

    void Framebuffer::bindColourAttachment(uint index)
    {
    }

    void Framebuffer::Invalidate()
    {
        if (mRendererID)
        {
            glDeleteFramebuffers(1, &mRendererID);
            glDeleteTextures(mColourAttachments.size(), mColourAttachments.data());
            glDeleteTextures(1, &mDepthAttachment);

            mColourAttachments.clear();
            mDepthAttachment = 0;
        }

        glCreateFramebuffers(1, &mRendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);

        bool multisample = mSpecification.samples > 1;

        if (mColourAttachmentSpecs.size())
        {
            mColourAttachments.resize(mColourAttachmentSpecs.size());
            Utils::CreateTextures(multisample, mColourAttachments.data(), mColourAttachments.size());

            for (usize i = 0; i < mColourAttachments.size(); i++)
            {
                Utils::BindTexture(multisample, mColourAttachments[i]);
                switch (mColourAttachmentSpecs[i])
                {
                case FramebufferTextureFormat::RGBA8:
                    Utils::AttachColourTexture(mColourAttachments[i], mSpecification.samples, GL_RGBA8, GL_RGBA, mSpecification.width, mSpecification.height, i);
                    break;

                case FramebufferTextureFormat::RED_INTEGER:
                    Utils::AttachColourTexture(mColourAttachments[i], mSpecification.samples, GL_R32I, GL_RED_INTEGER, mSpecification.width, mSpecification.height, i);
                    break;
                }
            }
        }

        if (mDepthAttachmentSpec != FramebufferTextureFormat::None)
        {
            Utils::CreateTextures(multisample, &mDepthAttachment, 1);
            Utils::BindTexture(multisample, mDepthAttachment);
            switch (mDepthAttachmentSpec)
            {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                Utils::AttachDepthTexture(mDepthAttachment, mSpecification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, mSpecification.width, mSpecification.height);
                break;
            }
        }

        if (mColourAttachments.size() > 1)
        {
            GL_ASSERT(mColourAttachments.size() <= 4);
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(mColourAttachments.size(), buffers);
        }
        else if (mColourAttachments.empty())
            glDrawBuffer(GL_NONE);

        GL_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    namespace Utils {

        GLenum TextureTarget(bool multisampled)
        {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }

        void CreateTextures(bool multisampled, uint* outID, int count)
        {
            glCreateTextures(TextureTarget(multisampled), count, outID);
        }

        void BindTexture(bool multisampled, uint id)
        {
            glBindTexture(TextureTarget(multisampled), id);
        }

        void AttachColourTexture(uint id, int samples, GLenum internalFormat, GLenum format, int width, int height, int index)
        {
            bool multisampled = samples > 1;
            if (multisampled)
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
        }

        void AttachDepthTexture(uint id, int samples, GLenum format, GLenum attachmentType, int width, int height)
        {
            bool multisampled = samples > 1;
            if (multisampled)
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
            }
            else
            {
                glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
        }

        bool IsDepthFormat(FramebufferTextureFormat format)
        {
            switch (format)
            {
            case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
            }

            return false;
        }
    }
}

#pragma warning(pop)